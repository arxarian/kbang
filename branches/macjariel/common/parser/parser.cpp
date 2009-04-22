/***************************************************************************
 *   Copyright (C) 2008 by MacJariel                                       *
 *   echo "badmailet@gbalt.dob" | tr "edibmlt" "ecrmjil"                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "parser.h"
#include "queryget.h"
#include "util.h"
#include "xmlnode.h"

#include <QtDebug>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "ioproxy.h"

#define PROTOCOL_VERSION "1.0"
#define ASSERT_SOCKET if (!mp_socket) { qDebug("Socket is dead!"); return; }



Parser::Parser(QObject* parent):
QObject(parent),
mp_ioProxy(0),
mp_socket(0),
m_streamInitialized(0),
m_readerState(S_Start),
m_readerDepth(0),
mp_parsedStanza(0),
mp_queryGet(0)

{
}

Parser::Parser(QObject* parent, QIODevice* socket):
QObject(parent),
mp_ioProxy(0),
mp_socket(0),
m_streamInitialized(0),
m_readerState(S_Start),
m_readerDepth(0),
mp_parsedStanza(0),
mp_queryGet(0)
{
    attachSocket(socket);
}

Parser::~Parser()
{
    if (mp_ioProxy != 0) delete mp_ioProxy;
}

void Parser::attachSocket(QIODevice* socket)
{
    Q_ASSERT(socket);
    if (mp_socket) detachSocket();
    mp_socket = socket;
    mp_ioProxy = new IOProxy(this);



    connect(mp_socket, SIGNAL(disconnected()),
            this, SLOT(detachSocket()));

    mp_streamReader = new QXmlStreamReader(mp_socket);
    //mp_streamWriter = new QXmlStreamWriter(mp_socket);
    mp_streamWriter = new QXmlStreamWriter(mp_ioProxy);


    connect(mp_ioProxy, SIGNAL(networkOut(const QByteArray&)),
            this, SLOT(writeData(const QByteArray&)));

    mp_streamWriter->setAutoFormatting(1);
    connect(mp_socket, SIGNAL(readyRead()),
            this, SLOT(readData()));


}

void Parser::detachSocket()
{
    if (!mp_socket) return;
    disconnect(mp_socket, SIGNAL(disconnected()),
               this, SLOT(detachSocket()));
    delete mp_streamWriter;
    delete mp_streamReader;
    delete mp_ioProxy;
    mp_ioProxy = 0;
    mp_socket = 0;
    emit terminated();
}

void Parser::writeData(const QByteArray& data)
{
    //qDebug() << ">>OUT>>" << data;
    mp_socket->write(data);
    emit outgoingData(data);
}



void Parser::initializeStream()
{
    ASSERT_SOCKET;
    sendInitialization();
}


QString Parser::protocolVersion()
{
    return PROTOCOL_VERSION;
}

void Parser::readData()
{
    //qDebug() << "<<IN<<" << mp_socket->peek(mp_socket->bytesAvailable());
    emit incomingData(mp_socket->peek(mp_socket->bytesAvailable()));
    while (!mp_streamReader->atEnd())
    {
        mp_streamReader->readNext();
        if (mp_streamReader->hasError()) streamError();
        if (!(mp_streamReader->isStartElement() ||
              mp_streamReader->isEndElement() ||
              (mp_streamReader->isCharacters() && !mp_streamReader->isWhitespace()))) continue;
/*
        qDebug() << this << "token: " << mp_streamReader->tokenType() <<
                            "name: " << mp_streamReader->name().toString() <<
                            "text: " << mp_streamReader->text().toString();
*/

        if (mp_streamReader->isEndElement()) m_readerDepth--;

        switch(m_readerState)
        {
        case S_Start:
            stateStart();
            break;
        case S_Ready:
            stateReady();
            break;
        case S_Stanza:
            stateStanza();
            break;
        case S_Terminated:
            break;
        case S_Error: // TODO
            break;
        }


        if (mp_streamReader->isStartElement()) m_readerDepth++;
    }
}

void Parser::stateStart()
{
    Q_ASSERT(m_readerDepth == 0);
    if (mp_streamReader->isStartElement() &&
        mp_streamReader->name() == "stream")
    {
        QString version = mp_streamReader->attributes().value("version").toString();
        if (version != Parser::protocolVersion())
        {
            qWarning("Protocol version mismatch.");
        }
        sendInitialization();
        emit streamInitialized();
        m_readerState = S_Ready;
    }
    else
    {
        m_readerState = S_Error;
    }
}
void Parser::stateReady()
{
    Q_ASSERT(m_readerDepth <= 1);
    if (mp_streamReader->isEndElement())
    {
//        qDebug("Recieved end of stream.");
        m_readerState = S_Terminated;
        sendTermination();
        mp_socket->close();
        return;
    }
    if (!mp_streamReader->isStartElement())
    {
        m_readerState = S_Error;
        return;
    }
    // Starting to read new stanza
    Q_ASSERT(mp_parsedStanza == 0);
    mp_parsedXmlElement = mp_parsedStanza = new XmlNode(0, mp_streamReader->name().toString());
    mp_parsedXmlElement->createAttributes(mp_streamReader->attributes());
    m_readerState = S_Stanza;
}

void Parser::stateStanza()
{
    if (mp_streamReader->isEndElement() && m_readerDepth == 1)
    {

        processStanza();
        // TODO: Process the stanza
//        mp_parsedStanza->debugPrint();
        delete mp_parsedStanza;
        mp_parsedStanza = mp_parsedXmlElement = 0;
        m_readerState = S_Ready;
        return;
    }
    if (mp_streamReader->isStartElement())
    {
        mp_parsedXmlElement = mp_parsedXmlElement->createChildNode(mp_streamReader->name(), mp_streamReader->attributes());
    }
    if (mp_streamReader->isEndElement())
    {
        mp_parsedXmlElement = mp_parsedXmlElement->parentNode();
    }
    if (mp_streamReader->isCharacters())
    {
        mp_parsedXmlElement->createChildTextNode(mp_streamReader->text());
    }

}

void Parser::processStanza()
{
    if (mp_parsedStanza->name() == "query")
    {
        const QString& id = mp_parsedStanza->attribute("id");
        if (mp_parsedStanza->attribute("type") == "get")
        {
            XmlNode* query = mp_parsedStanza->getFirstChild();
            if (!query) return;
            if (query->name() == StructServerInfo::elementName)
            {
                emit sigQueryServerInfo(QueryResult(mp_streamWriter, id));
                return;
            }
            if (query->name() == StructGame::elementName)
            {
                int gameId = query->attribute("id").toInt();
                emit sigQueryGame(gameId, QueryResult(mp_streamWriter, id));
                return;
            }
            if (query->name() == "gamelist")
            {
                emit sigQueryGameList(QueryResult(mp_streamWriter, id));
                return;
            }
        }
        if (mp_parsedStanza->attribute("type") == "result")
        {
            if (m_getQueries.contains(id))
            {
                m_getQueries[id]->parseResult(mp_parsedStanza->getFirstChild());
                m_getQueries[id]->deleteLater();
                m_getQueries.remove(id);
            }
        }


    }
    if (mp_parsedStanza->name() == "action")
    {
        XmlNode* action = mp_parsedStanza->getFirstChild();
        if (!action) return;
        if (action->name() == "create-game")
        {
            XmlNode* gameElem = 0;
            XmlNode* playerElem = 0;
            foreach(XmlNode* child, action->getChildren())
            {
                if (child->name() == "game" && gameElem == 0)
                {
                    gameElem = child;
                }
                if (child->name() == "player" && playerElem == 0)
                {
                    playerElem = child;
                }
            }
            if (!gameElem || !playerElem) return; // TODO: error handling
            StructGame game;
            game.read(gameElem);
            StructPlayer player;
            player.read(playerElem);
            emit sigActionCreateGame(game, player);
            return;
        }
        if (action->name() == "join-game")
        {
            int gameId = action->attribute("gameId").toInt();
            XmlNode* player = action->getFirstChild();
            if (!player) return; // Malformed join-game action
            StructPlayer p;
            p.read(player);
            emit sigActionJoinGame(gameId, p);
            return;
        }
        if (action->name() == "leave-game")
        {
            emit sigActionLeaveGame();
            return;
        }
        if (action->name() == "start-game")
        {
            emit sigActionStartGame();
            return;
        }
        if (action->name() == "message")
        {
            XmlNode* messageNode = action->getFirstChild();
            if (!messageNode || !messageNode->isTextElement()) return;
            emit sigActionMessage(messageNode->text());
            return;
        }
        if (action->name() == "draw-card") {
            int numCards = action->attribute("num-cards").toInt();
            bool revealCard = (action->attribute("reveal-card") == "true");
            emit  sigActionDrawCard(numCards, revealCard);
            return;
        }
        if (action->name() == "play-card") {
            ActionPlayCardData actionPlayCardData;
            actionPlayCardData.read(action);
            emit sigActionPlayCard(actionPlayCardData);
            return;
        }
        if (action->name() == "end-turn") {
            emit sigActionEndTurn();
            return;
        }
        if (action->name() == "pass") {
            emit sigActionPass();
            return;
        }
        if (action->name() == "discard-card") {
            int cardId = action->attribute("id").toInt();
            emit sigActionDiscard(cardId);
            return;
        }
    }

    if (mp_parsedStanza->name() == "event")
    {
        XmlNode* event = mp_parsedStanza->getFirstChild();
        if (!event) return;
        if (event->name() == "join-game")
        {
            XmlNode* player = event->getFirstChild();
            if (!player) return;
            int gameId = event->attribute("gameId").toInt();
            bool other = !event->attribute("other").isEmpty();
            bool creator = !event->attribute("creator").isEmpty();
            StructPlayer p;
            p.read(player);
            emit sigEventJoinGame(gameId, p, other, creator);
            return;
        }
        if (event->name() == "leave-game")
        {
            XmlNode* player = event->getFirstChild();
            if (!player) return;
            StructPlayer p;
            p.read(player);
            int gameId = event->attribute("gameId").toInt();
            bool other = !event->attribute("other").isEmpty();
            emit sigEventLeaveGame(gameId, p, other);
            return;
        }
        if (event->name() == "game-startable")
        {
            int gameId = event->attribute("gameId").toInt();
            bool startable = event->attribute("startable") == "1";
            emit sigEventGameStartable(gameId, startable);
            return;
        }
        if (event->name() == "start-game")
        {
            XmlNode* game = event->getFirstChild();
            StructGame x;
            StructPlayerList y;
            x.read(game, &y);
            emit sigEventStartGame(x, y);
            return;
        }
        if (event->name() == "game-sync") {
            GameSyncData gameSyncData;
            gameSyncData.read(event);
            emit sigEventGameSync(gameSyncData);
            return;
        }
        if (event->name() == "life-points") {
            int playerId = event->attribute("playerId").toInt();
            int lifePoints = event->attribute("lifePoints").toInt();
            emit sigEventLifePointsChange(playerId, lifePoints);
            return;
        }
        if (event->name() == "player-died") {
            int playerId = event->attribute("playerId").toInt();
            PlayerRole role = StringToPlayerRole(event->attribute("role"));
            emit sigEventPlayerDied(playerId, role);
            return;
        }


        if (event->name() == "card-movement")
        {
            CardMovementData x;
            x.read(event);
            emit sigEventCardMovement(x);
            return;
        }
        if (event->name() == "message")
        {
            XmlNode* messageNode = event->getFirstChild();
            if (!messageNode || !messageNode->isTextElement()) return;
            int senderId = event->attribute("senderId").toInt();
            QString senderName = event->attribute("senderName");
            emit sigEventMessage(senderId, senderName, messageNode->text());
        }
        if (event->name() == "game-context") {
            GameContextData gameContextData;
            gameContextData.read(event);
            emit sigEventGameContextChange(gameContextData);
        }
    }



}

void Parser::sendInitialization()
{
    if (m_streamInitialized) return;
    mp_streamWriter->writeStartDocument();
    mp_streamWriter->writeStartElement("stream");
    mp_streamWriter->writeAttribute("version", Parser::protocolVersion());
    mp_streamWriter->writeCharacters("");
    m_streamInitialized = 1;
}

void Parser::sendTermination()
{
    mp_streamWriter->writeEndElement();
    mp_streamWriter->writeEndDocument();
}


QueryGet* Parser::queryGet()
{
    QString id;
    do {
        id = randomToken(10, 10);
    } while (m_getQueries.contains(id));
    QueryGet* query = new QueryGet(this, mp_streamWriter, id);
    m_getQueries[id] = query;
    return query;
}

void Parser::eventJoinGame(int gameId, const StructPlayer& player, bool other, bool creator)
{
    eventStart();
    mp_streamWriter->writeStartElement("join-game");
    mp_streamWriter->writeAttribute("gameId", QString::number(gameId));
    if (other)
    {
        mp_streamWriter->writeAttribute("other", "1");
    }
    if (creator)
    {

        mp_streamWriter->writeAttribute("creator", "1");
    }
    player.write(mp_streamWriter);
    mp_streamWriter->writeEndElement();
    eventEnd();
}

void Parser::eventGameStartable(int gameId, bool startable)
{
    eventStart();
    mp_streamWriter->writeStartElement("game-startable");
    mp_streamWriter->writeAttribute("gameId", QString::number(gameId));
    mp_streamWriter->writeAttribute("startable", startable ? "1" : "0");
    mp_streamWriter->writeEndElement();
    eventEnd();
}

void Parser::eventStart()  { mp_streamWriter->writeStartElement("event");  }
void Parser::eventEnd()    { mp_streamWriter->writeEndElement();           }
void Parser::actionStart() { mp_streamWriter->writeStartElement("action"); }
void Parser::actionEnd()   { mp_streamWriter->writeEndElement();           }


void Parser::eventLeaveGame(int gameId, const StructPlayer& player, bool other)
{
    ASSERT_SOCKET;
    eventStart();
    mp_streamWriter->writeStartElement("leave-game");
    mp_streamWriter->writeAttribute("gameId", QString::number(gameId));
    if (other)
    {
        mp_streamWriter->writeAttribute("other", "1");
    }
    player.write(mp_streamWriter);
    mp_streamWriter->writeEndElement();
    eventEnd();
}

void Parser::eventStartGame(const StructGame& game, const StructPlayerList& players)
{
    ASSERT_SOCKET;
    eventStart();
    mp_streamWriter->writeStartElement("start-game");
    game.write(mp_streamWriter, &players);
    mp_streamWriter->writeEndElement();
    eventEnd();
}

void Parser::eventMessage(int senderId, const QString& senderName, const QString& message)
{
    ASSERT_SOCKET;
    eventStart();
    mp_streamWriter->writeStartElement("message");
    mp_streamWriter->writeAttribute("senderId", QString::number(senderId));
    mp_streamWriter->writeAttribute("senderName", senderName);
    mp_streamWriter->writeCharacters(message);
    mp_streamWriter->writeEndElement();
    eventEnd();
}

void Parser::eventGameContextChange(const GameContextData& gameContextData)
{
    ASSERT_SOCKET;
    eventStart();
    gameContextData.write(mp_streamWriter);
    eventEnd();
}

void Parser::eventGameSync(const GameSyncData& gameSyncData)
{
    ASSERT_SOCKET;
    eventStart();
    gameSyncData.write(mp_streamWriter);
    eventEnd();
}



void Parser::eventCardMovement(const CardMovementData& cardMovement)
{
    ASSERT_SOCKET;
    eventStart();
    cardMovement.write(mp_streamWriter);
    eventEnd();
}

void Parser::eventLifePointsChange(int playerId, int lifePoints)
{
    ASSERT_SOCKET;
    eventStart();
    mp_streamWriter->writeStartElement("life-points");
    mp_streamWriter->writeAttribute("playerId", QString::number(playerId));
    mp_streamWriter->writeAttribute("lifePoints", QString::number(lifePoints));
    mp_streamWriter->writeEndElement();
    eventEnd();
}

void Parser::eventPlayerDied(int playerId, PlayerRole role)
{
    ASSERT_SOCKET;
    eventStart();
    mp_streamWriter->writeStartElement("player-died");
    mp_streamWriter->writeAttribute("playerId", QString::number(playerId));
    mp_streamWriter->writeAttribute("role", PlayerRoleToString(role));
    mp_streamWriter->writeEndElement();
    eventEnd();
}

void Parser::streamError()
{
    if (mp_streamReader->error() == QXmlStreamReader::PrematureEndOfDocumentError) return;
    qDebug("Parser input error: %s", qPrintable(mp_streamReader->errorString()));
    m_readerState = S_Error;
}

void Parser::actionCreateGame(const StructGame& game , const StructPlayer& player)
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeStartElement("create-game");
    game.write(mp_streamWriter);
    player.write(mp_streamWriter, 1);
    mp_streamWriter->writeEndElement();
    actionEnd();
}


void Parser::actionJoinGame(int gameId, const QString& gamePassword, const StructPlayer& player)
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeStartElement("join-game");
    mp_streamWriter->writeAttribute("gameId", QString::number(gameId));
    if (!gamePassword.isEmpty()) mp_streamWriter->writeAttribute("gamePassword", gamePassword);
    player.write(mp_streamWriter, 1);
    mp_streamWriter->writeEndElement();
    actionEnd();
}

void Parser::actionStartGame()
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeStartElement("start-game");
    mp_streamWriter->writeEndElement();
    actionEnd();
}

void Parser::actionLeaveGame()
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeEmptyElement("leave-game");
    actionEnd();
}

void Parser::actionMessage(const QString& message)
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeStartElement("message");
    mp_streamWriter->writeCharacters(message);
    mp_streamWriter->writeEndElement();
    actionEnd();
}

void Parser::actionDrawCard(int numCards, bool revealCard)
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeStartElement("draw-card");
    mp_streamWriter->writeAttribute("num-cards", QString::number(numCards));
    mp_streamWriter->writeAttribute("reveal-card", revealCard ? "true" : "false");
    mp_streamWriter->writeEndElement();
    actionEnd();
}

void Parser::actionPlayCard(const ActionPlayCardData& actionPlayCardData)
{
    ASSERT_SOCKET;
    actionStart();
    actionPlayCardData.write(mp_streamWriter);
    actionEnd();
}

void Parser::actionEndTurn()
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeEmptyElement("end-turn");
    actionEnd();
}

void Parser::actionPass()
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeEmptyElement("pass");
    actionEnd();
}

void Parser::actionDiscard(int cardId)
{
    ASSERT_SOCKET;
    actionStart();
    mp_streamWriter->writeStartElement("discard-card");
    mp_streamWriter->writeAttribute("id", QString::number(cardId));
    mp_streamWriter->writeEndElement();
    actionEnd();

}



void Parser::terminate()
{
    sendTermination();
}








