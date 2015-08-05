#summary Protocol: events



# Introduction #
Server sends events to inform clients about changes. There are two types of events,]

# Server Events #
Those events are not directly related to gameplay. They can inform about another players entering/leaving game, etc.

## Enter Game ##
The _Enter Game_ event is received after client sends a request for entering a game. Client is able
to synchronize with the current state of the game, no matter whether the Game was already started or not.

The _game-id_ attribute specifies the id of the game. The _player-id_ attribute specifies the id of player, that the client controls.
If client connects to the game as a spectator, the _player-id_ attribute is not present (or is set to 0).

```
<server-event>
  <enter-game game-id="1" game-name="Game Name" game-state="waiting-for-players" player-id="1" last-event-id="0">
	<players>
	  <public-player id="1" name="Player #1" ... />
	  ...
	  <private-player id="1" role="deputy" ... />
	</players>
	<game-context ... />
	<graveyard>
		<card ... />
	</graveyard>
	<selection>
		<card ... />
		<card ... />
	</selection>
  </enter-game>
</server-event>
```


## Exit Game ##
The _Exit Game_ event is received after client leaves the game. This event is sent after client asks to leave the game,
but client can be kicked as well. The kicking is not implemented yet.

```
<server-event>
  <exit-game game-id="1" />
</server-event>
```

## Create Player ##
The _Create Player_ event is received when new player is created. This can happen either by a client connecting to the game
or by creating an AI. The nature of Bang! does not allow to create players in the middle of the game, so you can except this
event only in _waiting-for-players_ state.

```
<server-event>
  <create-player game-id="1">
	<public-player id="2" ... />
  </create-player>
</server-event>
```

## Remove Player ##
The _Remove Player_ event is received when a player is removed from the game. That can only happen in the _waiting-for-players-
state. When a client (who controls a player) leaves the game that was already started, the player is never removed. Instead the
player is in_detached_state and waits for another client/AI._

```
<server-event>
  <remove-player game-id="1" player-id="2" />
</server-event>
```

## Chat Message ##
The _Chat Message_ event is received when someone (something) sends a chat message. Chat messages currently work only in games,
but the concept will be extended to global chat messages and local chat messages. This needs to be discussed in the community.

Note that KBang does not support whispering and will never do.

```
<server-event>
  <chat-message game-id="1" client-id="1" player-id="1" sender-name="Player #1">
  Hello world!
  </chat-message>
</server-event>
```

## Game Update ##
The _Game Update_ event is received when some attributes of the game are changed, however only attributes not related to
gameplay are taken in account. This event is currently used only to tell, that _game startability_ changed, but in the future
it might be possible to change some attributes during the game (at least in the _waiting-for-players_ state) and this event
will serve the purpose.

```
<server-event>
  <game-update game-id="1" is-startable="true" />
</server-event>
```



# Game Events #
Game Events are events that are related to a concrete game. The list of all Game Events is a complete log of a game
and every game could be reconstructed from it. Game Events look like this:
```
<event>
  <game-event game-id="${GAME_ID}" event-id="1" type="${GAME_EVENT_TYPE}" $SIMPLE_GAME_EVENT_ATTRIBUTES>
    $COMPLEX_GAME_EVENT_ATTRIBUTES
    $LIST_OF_IMPERATIVE_COMMANDS
  </game-event>
</event>
```

To maintain the correct state of the game, only the _list of imperative commands_ is necessary. The rest of the game event
contains the semantic information, that is needed to display nice log message.

Game events are sent to all players and spectators, but because some information may be private to certain player, there
is a public and a private version of each game event.



## Start Game ##
Game started.

**Important:** When this event is received, the state of the game is set to _playing_.

```
<game-event game-id="1" type="start-game">
  <set-players>
      <player id="1" name="Anna" character="suzy-lafayette" role="unknown" life-points="4" />                                                                                       
      <player id="2" name="Bob" character="kit-carlson" role="unknown" life-points="4" />                                                                                           
      <player id="3" name="Chuck" character="willy-the-kid" role="sheriff" life-points="5" />                                                                                       
      <player id="4" name="Dave" character="vulture-sam" role="unknown" life-points="4" />                                                                                          
      <player id="5" name="Eve" character="calamity-janet" role="unknown" life-points="4" /> 
  </set-players>
</game-event>
```

## Finish Game ##
Game finished. The event contains the list of all players with their roles and information whether they are winners.

**Important:** When this event is received, the state of the game is set to _finished_.

```
<game-event game-id="1" type="finish-game">
  <update-player id="1" role="sheriff" is-winner="true" />
  <update-player id="2" role="deputy" is-winner="true" />
  <update-player id="3" role="outlaw" is-winner="false" />
  <update-player id="4" role="renegade" is-winner="false" />
  <update-player id="5" role="outlaw" is-winner="false" />
</game-event>
```

## Deal Cards ##
Cards were dealt to players.

```
<game-event game-id="1" type="deal-cards">
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="1">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="2">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="3">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="4">
    <card id="12345678" name="bang" type="playing" suit="spades" rank="ace" />
  </card-movement>

  ...
</game-event>
```

## Start Turn ##
Turn of the player started.

```
<game-event game-id="1" type="start-turn" player-id="1">
    <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="draw" />
</game-event>
```

## Check Deck ##
Player flipped a card from the deck, because of Jail or Dynamite.

### Successful Dynamite check ###
```
<game-event game-id="1" type="check-deck" player-id="1" success="true">
  <cause>
    <card id="12341234" name="dynamite" type="playing" suit="hearts" rank="king" />
  </cause>
  <checked>
    <card id="12345678" name="bang" type="playing" suit="hearts" rank="4" />
  </checked>
  <card-movement pocket-type-from="deck" pocket-type-to="graveyard" player-from="0" player-to="0">
    <card id="12345678" name="bang" type="playing" suit="hearts" rank="4" />
  </card-movement>
  <card-movement pocket-type-from="table" pocket-type-to="table" player-from="1" player-to="2">
    <card id="12341234" name="dynamite" type="playing" suit="hearts" rank="king" />
  </card-movement>
</game-event>
```

### Unsuccessful Dynamite check ###
```
<game-event game-id="1" type="check-deck" player-id="1" success="false">
  <cause>
    <card id="12341234" name="dynamite" type="playing" suit="hearts" rank="king" />
  </cause>
  <checked>
    <card id="12345678" name="bang" type="playing" suit="spades" rank="4" />
  </checked>
  <card-movement pocket-type-from="deck" pocket-type-to="graveyard" player-from="0" player-to="0">
    <card id="12345678" name="bang" type="playing" suit="spades" rank="4" />
  </card-movement>
  <card-movement pocket-type-from="table" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="12341234" name="dynamite" type="playing" suit="hearts" rank="king" />
  </card-movement>
</game-event>
```

### Successful Jail check ###
```
<game-event game-id="1" type="check-deck" player-id="1" success="true">
  <cause>
    <card id="12341234" name="jail" type="playing" suit="hearts" rank="king" />
  </cause>
  <checked>
    <card id="12345678" name="bang" type="playing" suit="hearts" rank="4" />
  </checked>
  <card-movement pocket-type-from="deck" pocket-type-to="graveyard" player-from="0" player-to="0">
    <card id="12345678" name="bang" type="playing" suit="hearts" rank="4" />
  </card-movement>
  <card-movement pocket-type-from="table" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="12341234" name="jail" type="playing" suit="hearts" rank="king" />
  </card-movement>
</game-event>
```

### Unsuccessful Jail check ###
```
<game-event game-id="1" type="check-deck" player-id="1" success="false">
  <cause>
    <card id="12341234" name="jail" type="playing" suit="hearts" rank="king" />
  </cause>
  <checked>
    <card id="12345678" name="bang" type="playing" suit="diamonds" rank="4" />
  </checked>
  <card-movement pocket-type-from="deck" pocket-type-to="graveyard" player-from="0" player-to="0">
    <card id="12345678" name="bang" type="playing" suit="diamonds" rank="4" />
  </card-movement>
  <card-movement pocket-type-from="table" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="12341234" name="jail" type="playing" suit="hearts" rank="king" />
  </card-movement>
</game-event>
```

### Lucky Duke check ###
The "draw!" of Lucky Duke must be split into 2 game events:

```
<game-event game-id="1" type="" player-id="1" use-ability="true">
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="0">
    <card id="12345678" name="bang" type="playing" suit="diamonds" rank="4" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="0">
    <card id="12341111" name="bang" type="playing" suit="hearts" rank="5" />
  </card-movement>
</game-event>

<game-event game-id="1" type="check-deck" player-id="1" success="true">
  <cause>
    <card id="12341234" name="jail" type="playing" suit="hearts" rank="king" />
  </cause>
  <checked>
    <card id="12341111" name="bang" type="playing" suit="hearts" rank="5" />
  </checked>
  <card-movement pocket-type-from="deck" pocket-type-to="graveyard" player-from="0" player-to="0">
    <card id="12341111" name="bang" type="playing" suit="hearts" rank="5" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="graveyard" player-from="0" player-to="0">
    <card id="12345678" name="bang" type="playing" suit="diamonds" rank="4" />      
  </card-movement>
  <card-movement pocket-type-from="table" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="12341234" name="jail" type="playing" suit="hearts" rank="king" />
  </card-movement>
</game-event>
```

## Draw Cards ##
Cards were drawn by a player to his hand.

```
<game-event game-id="1" type="draw-card" player-id="1">
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="4">
    <card id="12345678" name="bang" type="playing" suit="spades" rank="ace" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="4">
    <card id="87654321" name="beer" type="playing" suit="hearts" rank="ace" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

### Special case: Jesse Jones, Pedro Ramirez ###

```
<game-event game-id="1" type="draw-card" player-id="4" use-ability="true">
  <card-movement pocket-type-from="hand" pocket-type-to="hand" player-from="1" player-to="4">
    <card id="12345678" name="panic" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="4">
    <card id="87654321" name="beer" type="playing" suit="spades" rank="5" />
  </card-movement>
  <game-context current-player-id="4" requested-player-id="4" turn-number="1" game-play-state="turn" />
</game-event>
```

### Special case: Black Jack ###
The example shows Black Jack using his ability, as viewed by his opponents. They only see the second drew card.

```
<game-event game-id="1" type="draw-card" player-id="1" use-ability="true">
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="1">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="1">
    <card id="87654321" name="beer" type="playing" suit="hearts" rank="5" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="1">
    <card id="0" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

### Special case: Kit Carlson ###
Kit Carlson's drawing is split into three game events:
  1. Sending 3 cards into _selection_ pocket.
```
<game-event game-id="1" type="draw-card" player-id="1" use-ability="true">
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="1">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="1">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="1">
    <card id="0" />
  </card-movement>
</game-event>
```

> 2. Drawing the first card.
```
<game-event game-id="1" type="draw-card" player-id="1" use-ability="true">
  <card-movement pocket-type-from="selection" pocket-type-to="hand" player-from="0" player-to="1">
    <card id="0" />
  </card-movement>
</game-event>
```

> 3. Drawing the second card.
```
<game-event game-id="1" type="draw-card" player-id="1" use-ability="true">
  <card-movement pocket-type-from="selection" pocket-type-to="hand" player-from="0" player-to="1">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="selection" pocket-type-to="deck" player-from="0" player-to="0">
    <card id="0" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

**Note:** We might join the last two game events into one. It will allow to display only one log message in client,
instead of two. On the another hand, the player would have to send only one game action with both selected cards,
instead of two game actions, each with one card.


## Play Card ##
Player played a card.

### Blue bordered cards (without Jail) ###
```
<game-event game-id="1" type="play-card-on-table" player-id="1">
  <card-played>
    <card id="87654321" name="mustang" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="table" player-from="1" player-to="1">
    <card id="87654321" name="mustang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
</game-event>
```

### Jail ###
```
<game-event game-id="1" type="play-card-on-player" player-id="1" target-player-id="2">
  <card-played>
    <card id="87654321" name="jail" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="table" player-from="1" player-to="2">
    <card id="87654321" name="jail" type="playing" suit="hearts" rank="jack" />
  </card-movement>
</game-event>
```

### Bang ###
```
<game-event game-id="1" type="play-card-on-player" player-id="1" target-player-id="2">
  <card-played>
    <card id="87654321" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654321" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="2" turn-number="1" game-play-state="reaction" />
</game-event>
```

#### Missed on Bang ####
```
<game-event game-id="1" type="reaction-play-card" player-id="2">
  <card-played>
    <card id="87654321" name="missed" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="2" player-to="0">
    <card id="87654321" name="missed" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

#### Pass on Bang ####
```
<game-event game-id="1" type="reaction-pass" player-id="2">
  <life-points player-id="2" life-points="3" />
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

#### Barrel on Bang ####
```
<game-event game-id="1" type="check-deck" player-id="2" success="true">
  <cause>
    <card id="12345678" name="barrel" type="playing" suit="hearts" rank="5" />
  </cause>
  <checked>
    <card id="12341111" name="bang" type="playing" suit="hearts" rank="5" />
  </checked>
  <card-movement pocket-type-from="deck" pocket-type-to="graveyard" player-from="0" player-to="0">
    <card id="12341111" name="bang" type="playing" suit="hearts" rank="5" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

#### Jourdonnais's "barrel" on Bang ####
```
<game-event game-id="1" type="check-deck" player-id="2" use-ability="true" success="true">
  <cause>
    <card id="0" name="jourdonnais" type="character" />
  </cause>
  <checked>
    <card id="12341111" name="bang" type="playing" suit="hearts" rank="5" />
  </checked>
  <card-movement pocket-type-from="deck" pocket-type-to="graveyard" player-from="0" player-to="0">
    <card id="12341111" name="bang" type="playing" suit="hearts" rank="5" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

### Beer ###
```
<game-event game-id="1" type="play-card" player-id="1">
  <card-played>
    <card id="87654321" name="beer" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654321" name="beer" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <life-points player-id="1" life-points="4" />
</game-event>
```

### Saloon ###
```
<game-event game-id="1" type="play-card" player-id="1">
  <card-played>
    <card id="87654321" name="saloon" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654321" name="saloon" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <life-points player-id="1" life-points="4" />
  <life-points player-id="2" life-points="3" />
  <life-points player-id="5" life-points="4" />
</game-event>
```

### Wells Fargo, Diligenza ###
```
<game-event game-id="1" type="play-card" player-id="2">
  <card-played>
    <card id="87654321" name="wells-fargo" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="2" player-to="0">
    <card id="87654321" name="wells-fargo" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="2">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="2">
    <card id="0" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="hand" player-from="0" player-to="2">
    <card id="0" />
  </card-movement>
</game-event>
```

### General Store ###
```
<game-event game-id="1" type="play-card" player-id="2">
  <card-played>
    <card id="87654321" name="general-store" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="2" player-to="0">
    <card id="87654321" name="general-store" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="0">
    <card id="12332111" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="0">
    <card id="12332112" name="missed" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="0">
    <card id="12332113" name="missed" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <card-movement pocket-type-from="deck" pocket-type-to="selection" player-from="0" player-to="0">
    <card id="12332114" name="beer" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="2" requested-player-id="2" turn-number="1" game-play-state="reaction" reaction-type="general-store" />
</game-event>
```

```
<game-event game-id="1" type="draw-card" player-id="2">
  <card-movement pocket-type-from="selection" pocket-type-to="hand" player-from="0" player-to="2">
    <card id="12332113" name="missed" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="2" requested-player-id="3" turn-number="1" game-play-state="reaction" reaction-type="general-store" />
</game-event>
```

```
<game-event game-id="1" type="draw-card" player-id="3">
  <card-movement pocket-type-from="selection" pocket-type-to="hand" player-from="0" player-to="3">
    <card id="12332114" name="beer" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="2" requested-player-id="4" turn-number="1" game-play-state="reaction" reaction-type="general-store" />
</game-event>
```

```
<game-event game-id="1" type="draw-card" player-id="4">
  <card-movement pocket-type-from="selection" pocket-type-to="hand" player-from="0" player-to="4">
    <card id="12332111" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="2" requested-player-id="1" turn-number="1" game-play-state="reaction" reaction-type="general-store" />
</game-event>
```

```
<game-event game-id="1" type="draw-card" player-id="1">
  <card-movement pocket-type-from="selection" pocket-type-to="hand" player-from="0" player-to="1">
    <card id="12332112" name="missed" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="2" requested-player-id="2" turn-number="1" game-play-state="turn" />
</game-event>
```

### Panic, Cat Balou on player's hand ###
```
<game-event game-id="1" type="play-card-on-player" player-id="1" target-player-id="2">
  <card-played>
    <card id="87654321" name="panic" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654321" name="panic" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <card-movement pocket-type-from="hand" pocket-type-to="hand" player-from="2" player-to="1">
    <card id="12332112" name="missed" type="playing" suit="hearts" rank="jack" />
  </card-movement>
</game-event>
```

### Panic, Cat Balou on player's table ###
```
<game-event game-id="1" type="play-card-on-card" player-id="1">
  <card-played>
    <card id="87654321" name="panic" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-target>
    <card id="12312311" name="volcanic" type="playing" suit="spades" rank="jack" />
  </card-target>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654321" name="panic" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <card-movement pocket-type-from="table" pocket-type-to="hand" player-from="2" player-to="1">
    <card id="12312311" name="volcanic" type="playing" suit="spades" rank="jack" />
  </card-movement>
</game-event>
```

### Duel ###
```
<game-event game-id="1" type="play-card-on-player" player-id="1" target-player-id="2">
  <card-played>
    <card id="87654321" name="duel" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654321" name="duel" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="2" turn-number="1" game-play-state="reaction" reaction-type="duel" />
</game-event>
```

```
<game-event game-id="1" type="reaction-play-card" player-id="2">
  <card-played>
    <card id="12314637" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="2" player-to="0">
    <card id="12314637" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="reaction" reaction-type="duel" />
</game-event>
```

```
<game-event game-id="1" type="reaction-play-card" player-id="1">
  <card-played>
    <card id="12314631" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="12314631" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="2" turn-number="1" game-play-state="reaction" reaction-type="duel" />
</game-event>
```

```
<game-event game-id="1" type="reaction-pass" player-id="2">
  <life-points player-id="2" life-points="3" />
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

### Indians, Gatling ###
```
<game-event game-id="1" type="play-card" player-id="1">
  <card-played>
    <card id="87654321" name="indians" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654321" name="indians" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="2" turn-number="1" game-play-state="reaction" reaction-type="indians" />
</game-event>
```

```
<game-event game-id="1" type="reaction-play-card" player-id="2">
  <card-played>
    <card id="87654344" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="2" player-to="0">
    <card id="87654344" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="3" turn-number="1" game-play-state="reaction" reaction-type="indians" />
</game-event>
```

```
<game-event game-id="1" type="reaction-pass" player-id="3">
  <life-points player-id="3" life-points="1" />
  <game-context current-player-id="1" requested-player-id="4" turn-number="1" game-play-state="reaction" reaction-type="indians" />
</game-event>
```

```
<game-event game-id="1" type="reaction-play-card" player-id="4">
  <card-played>
    <card id="87654111" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-played>
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="4" player-to="0">
    <card id="87654111" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="turn" />
</game-event>
```

## Discard Card ##
```
<game-event game-id="1" type="discard-card" player-id="1">
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654111" name="bang" type="playing" suit="hearts" rank="jack" />
  </card-movement>
  <game-context current-player-id="1" requested-player-id="1" turn-number="1" game-play-state="discard" />
</game-event>
```

```
<game-event game-id="1" type="discard-card" player-id="1">
  <card-movement pocket-type-from="hand" pocket-type-to="graveyard" player-from="1" player-to="0">
    <card id="87654114" name="missed" type="playing" suit="hearts" rank="jack" />
  </card-movement>
</game-event>
```


# Request for comments #
I would like to have a discussion about the game events.