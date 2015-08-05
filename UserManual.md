# Introduction #
KBang is a [cross-platform](http://en.wikipedia.org/wiki/Cross-platform) [open source](http://en.wikipedia.org/wiki/Open_source) implementation of [Bang!](http://en.wikipedia.org/wiki/Bang!), the card game designed by Emiliano Sciarra. The goal of this project is to allow Bang! enthusiasts play the game online with another folks from around the world.



# Installation #
KBang supports several platforms and operating systems. Unfortunately there is only Windows build now - for other platforms you have to compile sources yourself. This will hopefully change in the future, as we plan to support various Linux distributions, as well as provide Mac OS X binaries.

# Windows #
The preferred method is to use the binary package, although you can compile KBang yourself (for example to verify a bugfix in the SVN).

## Precompiled package ##
The Windows version of KBang is distributed in a zipped archive. This archive contains compiled KBang binaries for both client and server, graphics files and bundled Qt4 runtime files. To install KBang on your Windows, simply unpack the archive to a folder. You can then execute one of the _exe_ files to run client or server.

## Compiling from sources ##
If you want to, you can compile KBang from sources by yourself. First of all get the sources. You can get them either from the download section, or from SVN.

### Checking-out sources from SVN ###
If you are not familiar with SVN, I recommend you to download and install [TortoiseSVN](http://tortoisesvn.net/downloads). After you install it, it will extend the context menu in Windows.

Go to the folder where you want to download the sources and with right click open the context menu. From the context menu select **SVN checkout**. Enter **http://kbang.googlecode.com/svn/trunk/* to the _URL of repository_ field and press _OK_. It should download the sources now.**

In the future you can update the repository by the **SVN update** option from the context menu.

### Compiling with MinGW ###
The easiest way how to compile KBang on Windows is to use the **Qt SDK for Windows**. You can download it from the [QtSoftware download site](http://www.qtsoftware.com/downloads). After you have installed Qt SDK, run **Qt Creator** and open the **kbang.pro** file, that is located in the root directory of the sources. Then click **Build all** from the **Build** menu.

When the compilation is finished (the small vertical bar on the left panel is green), it should create files **kbang-client.exe** and **kbang-server.exe** in debug or release subdirectory.

To run those files, you need to supply gfx directory and dll files. The best thing is to use them from the binary package. Just download the latest binary package, unzip it somewhere and replace the exe files with those compiled ones.

### Compiling with Visual Studio ###
Visual Studio is not currently supported, but I would appreciate if someone would like to make it work. I am afraid that there need to be some fixes in code to be done to make it compile. If you send me the compile log with errors, I can fix them in SVN.

_Note that KBang supports Windows XP and newer. You can experience problems (or not be able to run KBang at all) on older Windows._

# Linux #
At the current state we do not provide Linux binaries so you need to compile KBang yourself.

## Compiling from sources ##
First you need to get source files. You can get them either from the download section, or from SVN.

You also need Qt4 library, **version 4.4** or newer.

On Debian/Ubuntu:
```
$ sudo apt-get install libqt4-dev
```

On Gentoo:
```
$ emerge qt:4
```

Then download the sources and unpack them into a directory.
```
$ tar xzf kbang-x.y.z.tar.gz
$ cd kbang-x.y.z
```

Finally compile the sources.
```
$ qmake kbang.pro
$ make
```

**Note:** if you have both Qt3 and Qt4 installed, you may need to use **qmake-qt4** instead of **qmake**. If you use Qt3 version of qmake, the compilation will fail on missing header files (QIODevice, ...).

**Note:** if you have older version of Qt than 4.4, your compiler will probably complain about missing **removeOne** method in **QList** template class. Please update to Qt 4.4 or newer to solve this problem.

You can then execute client or server.
```
$ ./kbang-client
$ ./kbang-server
```


# Client #
For end users the most interesting part of KBang project would be the _client_. If you want to play over the Internet, you do not have to run the server yourself and connect to the official KBang server **alderan.cz**.

## Connecting to server ##
After you start the client, you need to connect to a server. Select _Connect to server_ from _Game_ menu and the server dialog will pop up. To play over the internet you can connect to **alderan.cz**, which is the default server. If you do not have access to the internet, you can still play on your LAN or run the server yourself. To connect to the server that runs on your machine, add a server with address **localhost**.

## Creating games ##
When you connect to a server, you can create a new game. Select **Create new game** from the **Game** menu. In the dialog that pops up you can set the parameters of the game:
  * _Game Name_ - the game will be listed under this name.
  * _Game Description_ - additional description of the game, can be left empty.
  * _Number of Players_ - the expected minimal and maximal count of players. The game can be started only if enough players connect.
  * _Number of AI Players_ - set this number if you want to have AI players in your game.
  * _Max. Number of Spectators_ `(not implemented yet)` - you can limit the number of spectators with this option.
  * _Password for Players_ - this option allows you to protect your game with password. Players that want to join your game need to know this password. Leave this option empty to disable password.
  * _Password for Spectators_ - this option allows you to protect your game with password. Anyone that wants to spectate your game need to know this password. Leave this option empty to disable password.
  * _Order of Players_ - if set to _chronological_, the order of players around the table is kept as the players connect to the game. If set to _random_, players are shuffled after the game starts.

When you create a new game, you automatically join it as a new player. You can set the player properties as well. This properties are described in the _Joining games_ section.

## Joining games ##

You can join a game by selecting **Join Game** option from the **Game** menu. The _Join Game_ dialog appears. In the left part you can see all the games on the server. If you select a game from the list, you can see the information about the game and the list of players in the left part of the dialog.

To join a game, select the game and then select a player that you want to join to. If the game is in the _waiting_ state, you can create a new player. In this case, select _Create new player_ in the player list.

Finally you can set the player properties:
  * _Player Name_ - the name of the player.
  * _Player Password_ - the password of the player. This password is used in case of reconnecting to a player. For example if you by an accident disconnect from a game, you can reconnect back and continue in game. To prevent misuse of this feature, you need to enter the same password as before.
  * _Avatar_ - you can set an avatar which is then displayed next to your name in game. Click on the **Avatar** area and select the file with your avatar. The image is automatically resized.

## Gameplay ##

Every game starts in the _waiting_ state. As soon as enough players connect to the game, it can be started by the creator by pressing the **Start** button in the middle of the table. The cards are then dealt and the game starts.

The further text assumes that you are familiar with the rules of the Bang! game. If you are new to Bang!, please read the official rules. They can be found on the [official Bang! site](http://www.davincigames.com/page_eng.cfm?sez=01&gioco=bang!) in various languages.

### Zooming cards ###
If you want to read the text on a card, you can zoom it by clicking on it with right mouse button.

### Beginning of the turn ###
The turn typically starts by drawing two cards from the deck. You can do this by clicking on the deck. If you have Jail (or Dynamite) in front of you, you need to "Draw!" to check its effect. You can do it by clicking on the Jail (or Dynamite) card. Note that you always check the Dynamite first.

Certain characters have the ability to draw differently:
  * _Kit Carlson_ may draw 3 cards, select 2 of them and return the remaining back to the top of the deck. To use this ability, click on the Kit Carlson card, instead of clicking the deck. Three cards then gather in the middle of the table. Click on the cards that you want to take.
  * _Pedro Ramirez_ may draw the first card from the top of the discard pile. To use this ability, click on the Pedro Ramirez card, instead of clicking the deck.
  * _Jesse Jones_ may draw the first card from the hand of arbitrary player. To use this ability, click on the Jesse Jones card and select the target player.

### Main phase ###
In the main phase you can play cards from your hand. You can play most of the cards by simply clicking on them. To play a card that requires to specify target player or target card, click the card and then click on the target player or target card.

### Reactions ###
When someone plays a card that effects you, you can either decide not to react by clicking the **Pass** button or you can counter the effect by another card by clicking on that card. To use Barrel, simply click it. Do not forget, that Barrel can be played only against Bang! and Gatling.

You can also use the Jourdonnais ability. Jourdonnais is considered to have one implicit barrel. To draw a card on that barrel, click on the Jourdonnais card.

When someone plays General Store, you can pick a card by clicking on it.

### End of turn ###
If you want to end your turn, click on the **End Turn** button. If you have more cards in your hand than your life points, you need to discard some cards. Click on the **Discard** button and then select cards you want to discard.

### Using character abilities ###
Most of the character abilities are applied automatically, so you do not have to remember them. However some of them are voluntary, so you can use them only if you want. Some of the character abilities have been discussed in corresponding sections above. The following character abilities that need to be activated are:
  * _Sid Ketchum_ - he can discard two cards and to regain one life point. To use this ability, click on the Sid Ketchum card and select two cards from your hand. 