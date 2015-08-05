# Introduction #

Why do we need a new architecture? KBang is a game played by players from various countries and people prefer to have Bang! cards localized to their languages. Moreover, we can have alternative (free) card graphics in the future (to avoid copyright issues). That's why it's handy to have an architecture that would allow to easily change between card graphics or even automatically download new graphics from server.

# Structure of files #

CardSets are stored in a directory in `$KBANG_DATA/cardsets/$CARDSET_NAME`. Every CardSet has a `cardset.xml` file:

```
<?xml version="1.0" encoding="UTF-8"?>
<cardset name="bang-original-v3-cs_CZ" locale="cs_CZ" slot="bang-original" render-signs="true" render-signs-position="50,500">
    <card name="bang" type="playing" suit="diamonds" rank="8-A" gfx="cards/bang1.png" /> 
    <card name="bang" type="playing" suit="spades" rank="A" gfx="cards/bang1.png" />
    <card name="bang" type="playing" suit="clubs" rank="2-6" gfx="cards/bang2.png" /> 
    <card name="bang" type="playing" suit="hearts" rank="Q-A" gfx="cards/bang2.png" /> 
    <card name="bang" type="playing" suit="clubs" rank="7-9" gfx="cards/bang2.png" /> 

    <card name="missed" type="playing" gfx="cards/missed.png" />
    <card name="beer" type="playing" gfx="cards/beer.png" />
    <card name="saloon" type="playing" gfx="cards/saloon.png" />
    <card name="wellsfargo" type="playing" gfx="cards/wellsfargo.png" />
    <card name="diligenza" type="playing" gfx="cards/diligenza.png" />
    <card name="generalstore" type="playing" gfx="cards/generalstore.png" />
    [...]

    <suit name="diamonds" gfx="suits/diamonds.png" />
    [...]

    <rank name="A" gfx="ranks/A.png" />
    [...]

</cardset>
```

This approach will allow us:
  * To have more graphics for same cards (currently we have only one graphics for Bang! card, instead of two)
  * To decide whether render signs (suits and ranks) in runtime, or not. (render-signs attribute). The value in cardset tag sets the default behaviour which can be overwritten with render-signs attribute in card tag. _(this is not implemented and will be only if it's necessary)_
  * When we decide to render signs, we now use graphics for them, instead of using fonts to draw them.

The gfx parameter points to the graphics file that is stored in the CardSet directory.

## Slots ##
In the future we are going to introduce expansions into KBang. When client connects to a new game, the server tells the client which slots are necessary (for example: bang-original, bang-dodgecity). Client then loads some CardSets (according to preferences) and makes sure that all slots are satisfied.

The loaded sets are then merged together and card graphic information is then provided to CardWidget instances to draw cards. Card names should be unique even across different slots.

# Downloading card sets from server #
Server may allow to download some cardsets.

## Getting the list of available cardsets ##

**client:**
```
<query id="42" type="get">
  <cardset-list />
</query>
```

**server:**
```
<query id="42" type="result">
  <cardset-list>
    <cardset name="bang-original-v3-cs_CZ" locale="cs_CZ" slot="bang-original" />
    <cardset name="bang-original-v3-en_US" locale="en_US" slot="bang-original" />
    [...]
  </cardset-list>
</query>
```

## Downloading a cardset ##

**client:**
```
<query id="42" type="get">
  <cardset name="bang-original-v3-cs_CZ" />
</query>
```

**server:**
```
<query id="42" type="result">
  <cardset name="bang-original-v3-cs_CZ">
    <file name="cardset.xml">[base-64 encoded data]</file>
    <file name="cards/appaloosa.png">[base-64 encoded data]</file>
    [..]
  </cardset>
</query>
```