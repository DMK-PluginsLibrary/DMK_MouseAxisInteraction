# DMK-BasicAxisInteractions
This plugin aims to hanlde simple inputs. 
Main goals are:
1. Easy to setup
2. Maxmimalisation of customisation
3. ablity to be use for more analog game interaction

## Installation
You should simply download release, extract it in your `<Project>/Plugins` folder (you may need to create `Plugins` folder in your project) and start or restart editor.

## Setup
Plugins requires minimal setup that is:
1. In order for character to be able to interact, we need to add to him component.
    We have currently two components one is implemented in cpp and should be more performant `BP_DMKInteraction_CPP` and one that showcase internal component logic `BP_DMKInteraction_Showcase`. Both are working and you can pick any of them based on if and how you want to extend their logic.
2. Any interactable object need to either implement `DMK Interaction Target Interface` or add components that have already it implemented like `DMK_InteractionBoxComponent` or it's child for instance `BP_KnobLogic` that showcase how to handle interaction logic
3. Adding neccesary input mapping contexts, you can look how `BP_DMKCharacter` or `BP_DMKCharacter_CPP` are setup (diffrence between them is just which component they use)

## Components Logic
### PlayerInteraction
`BP_DMKInteraction_CPP` and `BP_DMKInteraction_Showcase` are just firing line trace on visual channel to test if they hit any object that implements interface `DMK Interaction Target Interface`, then if we press interaction we will interact with this object.
Interaction is passed by player component into target component and we can classify couple stages of interactions:
1. `Hold interaction started` -> when we start interacting with object (for instance pressing left mouse button)
2. `Stop Hold interaction` -> when we release interaction button
3. `Press interaction` -> this action if fired when we press button and leave it in time treshold that this interaction is not counted as hold
4. `Hold interaction axis` -> this is happening when we are holding button and we input some additional values using axis for instance we can hold door and move mouse around to add axis input and manipulate doors like in Amnesia games.

There are couple imporant fields you may want to change:
- Interaction Base Component -> this defines what component (it's location and rotation) should be use for line trace
- TraceLenght -> defines how long should be trace
- Should Ignore owner -> should this trace stop at owner or ignore it?
- Should Check Direction -> it is changing input based on direction of both player and interacted item
- What movement it should return to after blockign movement (blocking movement is defined in interaction target component)

This is nearly all important pieces of logic. Note that only discticion between press and hold is how long we are holding button.

### Interaction Target
Basic interaction target is `DMK_InteractionBoxComponent` that is `Box Collision component` that have already implemented interface with blueprint bindings and have ability to be extended with logic. Extension is simple we just create child blueprint and `BP_KnobLogic` is good example of how to do it. 

There are couple imporant fields you may want to change:
- ShouldBlockMovement -> should interaction start blocks movement of character?
- What is treshold for this to be consider just press interaction
- What input mapping context should be add (for Axis input)



## Examples
There is only one small example of how plugin works
- `BP_DMKCharacter_CPP` we use this pawn as it have already all neccesary setups
- `BP_DMK_Door` we place it into level as this is already setup to be amnesia like physical doors with knob to open
We can test feeling of interaction


