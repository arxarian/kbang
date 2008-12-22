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
#ifndef CARDABSTRACT_H
#define CARDABSTRACT_H

#include <QObject>
#include "game.h"
#include "parser/parserstructs.h"

class Player;

/**
 * The base class for all cards that are in the desk. Character and role
 * cards are not included.
 * @author MacJariel <MacJariel@gmail.com>
 */
class CardAbstract: public QObject
{
Q_OBJECT
public:
    CardAbstract(Game *game, int id);
    virtual ~CardAbstract();

    inline void setOwner(Player *owner) { mp_owner = owner; }
    inline void setPocket(const Pocket& pocket) { m_pocket = pocket; }

    inline int id() const { return m_id; }
    virtual QString type() const = 0;
    inline StructCardDetails cardDetails() const { return StructCardDetails(m_id, type()); }
    inline Player* owner() const { return mp_owner; }
    inline Pocket  pocket() const { return m_pocket; }

protected:
    Game *mp_game;

private:
    Player *mp_owner;
    Pocket  m_pocket;
    const int m_id;
};

#endif
