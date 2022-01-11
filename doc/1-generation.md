# Maze generation

This section goes more in depth with maze generation. In this section, "designer" refers to a human who controls the creation process.

## Overview

Generation is done by executing each of these steps.
Each step can be bypassed so that the designer can implement it by himself, or can be computer-generated.
Each step has an input that is (obvsiouly) the output of the previous step, plus other inputs which are either computed or provided by the designer.

| Step name | What the step represents | Output | Status<br>(if previous step to here is currently implemented) |
| --------- | ------------------------ | ------ | --------------------------------------------------------- |
| Level Strategy | Highest-level view which gives difficulty, ambiance, rythm | JSON file of strategy |  |
| Story | Description of steps that happen: connection between steps, what they contain.<br>Those steps describes the main events happening in a game, like opening a door and giving time points. This could be a "story" told by a human. | JSON file of game play<br>Viz thru Plantuml | Not implemented |
| Rooms logic | Listing of rooms & gates playable definition:<br><ul><li>structure of a room from a playable aspect: break-down of rooms in containers</li><li>connections of containers between them&nbsp;</li><li>rooms content : gates, objects, triggers, events</li></ul> | JSON file of level with list of rooms with their containers<br>Viz thru Plantuml | Not implemented |
| Element instantiation | Break-down of containers into bricks from their containers<br>Contrary to containers, bricks are tree-ordered and are typed. At this step, only their required characteristics are provided.<br>Same for gate, but simpler, as there is no tree-like structure (no containers) | JSON file for each room<br><br>This starts to be navigable if next step is done automatically, although it's ugly and uniform. | Yes |
| Element personalization | Each brick and gate merges "public" parameters provided by higher-level and/or designer, with missing parameters, to generate "private" parameters.<br><br>Once this step is done it must generate the exact same maze in the next steps, from a logical perspective. | JSON file for each room<br><br>This is navigable, although missing last step.<br><br>glTF format. | Yes |
| Dressing instantiation | Generate a general draft of "dressing" of rooms; dressings are the way the details appear, like texturing. It can also change the points and can break down the faces. This is done with adaptors to add a little final touch of randomness and personnality to each graphical elements. | Playable level. glTF format. | Yes |
| Dressing personalization | Each dressing merges "public" parameters provided by higher-level and/or designer, with missing parameters, to generate "private" parameters.<br><br>Once this step is done it must generate the exact same visual rendering in the next steps. | Playable level. glTF format. | Yes |

Note that the designer is given the ability to finely control every generation step through parameters. However, it is still possible to let the system handle all details with random generation.

## Level strategy

### Rationale

TBD

### Definitions

TBD

### Inputs

TBD

### Output

TBD

## Level steps

### Rationale

TBD

### Definitions

TBD

### Inputs

TBD

### Output

TBD

## Rooms Logic

### Rationale

The goal is to represent the level from a structural point-of-view:

* list of rooms
* how they connect with one another through gates
* how they are logically broken down in containers, each with triggers, events and gates

From this point, the information about the gameplay is transformed into an almost real maze: it is possible to generate a map-like graphical representation of it where rooms appears and a player could virtually play.
This part can be completely hand-written, or generated from the previous steps.

### Definitions

| Name | Definition |
| ---- | ---------- |
| Room | This is an euclidean space made of containers.<br>Rooms have an ID, unique at the whole level. |
| Container | A container is the element that will give birth to a list of s (see next step).<br>It has structural requirements, notably the connections with the other containers. It is possible to go from one container to another only when a connection is defined.<br>A container will contain<br><ul><li>a list of gates (can be void)</li><li>a list of objects (can be void)</li><li>a list of triggers.</li></ul>Containers have an ID, unique at the room level. |
| Event | This is a binary value defined at the level scale. They can be changed by triggers and have impacts on connections.<br>(TBD: it should also be usable by gravity computations)<br>Events have an ID, unique at the whole level. |
| Connection | A link between 2 containers. It can be:<br><ul><li>visible/invisible</li><li>traversable/untraversable</li></ul>Both of those properties can depend on events values |
| Trigger | A structural element that the player can interact with.<br>It can be:<br><ul><li>visible/invisible</li><li>voluntary/unvoluntary</li></ul>A trigger changes the value of an event |
| Gate | A link between 2 rooms. Gates have an IN and an OUT. It's just a way to be sure to place them, as they can be asymmetrical.<br>Gates have an ID, unique at the whole level. |
| Object | Any non-structural object that can move and that the player may play with: bonuses, maluses, ... |

### Inputs

It can be hand-written or computed.

ATM, it is not possible to compute them from the previous step. However, it is expected to be able to follow a "strategy" (TBD) provided elsewhere that builds this step.

### Output

The output is a level description with:

* a list of containers definitions for every room
* a list of gates definitions

It is possible to generate a graphical visualization of this output.

## Element Instantiation

### Rationale

The goal is to give the main "personality" of the room, by replacing containers by a fixed number of bricks with characteristics that allow to implement the containers requirements.

The same is applied to gates, but this is done for the whole level.

### Definitions

| Name | Definition |
| ---- | ---------- |
| Brick class | This is the real type of the given brick, it gives it its rough appearance, although still customizable through parameters.<br>Examples are:<br><ul><li class="">For closed containers: a rectangular room, a sphere, a corridor, a donut...</li><li class="">For opened containers: a rectangular space filled with boxes, a stair, a lift, ...</li></ul>The list of bricks can be augmented easily through new classes, this is explained in <span class="colour" style="color:rgb(212, 212, 212)">[</span><span class="colour" style="color:rgb(206, 145, 120)">3 Writing new mazes elements</span><span class="colour" style="color:rgb(212, 212, 212)">](<u>doc/3-writing\_maze\_elements.md</u>)</span> |
| Public parameters section | This is a list of main parameters that define the brick, augmented with parameters forced by the designer |
| Private parameters section | This is the whole list of parameters that the brick requires. Everything that is not in the public parameters is set randomly. This is defined in the next section. |
| Bricks Strategy | A class that allows to transform a container definition to a sub-space list with characteristics.<br>Each strategy has its own limits, so it must be selected if and only if it handles the container definition.<br>Most basic ones can only handle a room with 1 container and N gates<br>Most complex ones could handle anything.<br>The list of strategies can be aumgented through new classes. |
| Personalization Strategy | A class that allows to choose a brick type from a list of characteristics.<br>A strategy can choose amongst a limited number of brick types, or use the brick types limitations to choose randomly.<br>The list of strategies can be aumgented through new classes. |
| Gate class | Same logic for the gate (although much simpler). Note that gates must fulfill requirements for 2 rooms at the time ! |

### Inputs

* The previous room definition logic
* A bricks strategy that can be forced by the designer.
* The strategy provides the output.
* The strategy can be specified, otherwise it is randomly chosen.

### Output

The output is a new room description in its dedicated directory with:

* a list of bricks definitions with their types and main

Each gate has its own definition, too, in a dedicated directory.
It is possible to generate a graphical visualization of this output.

## Element Personalization

### Rationale

From the previous step, the bricks were given general parameters, but there are still some parameters that can be personalized. They can be either chosen by the designer in the "public" section, or the brick chooses defaults. In the end, everything is merged in the "private" section.

### Inputs

* Bricks types
* Bricks public parameters

### Output

Brick private parameters

## Dressing Instantiation

### Rationale

Bricks generate faces (walls, floor, ...) with generic parameters, based on the object they are requested to model (a wall, a tunnel, ...)
Those faces can be modified, split, etc... as long as they respect the original intention of the brick. They must also be textured.
All those steps is the job of a dressing class. This step chooses dressing classes for all the bricks, either manually or automatically through a dressing class.

### Definitions
<br>
| Name | Definition |
| ---- | ---------- |
| Face | This is a convex polygon, with 2 sets of characteristics about its usage:<br><ol><li>physical: is it rough ? with which properties ?</li><li>visual: what's its meaning ? a wall, a floor, a ceiling, a door ?</li></ol>Bricks generate lists of faces that the dressing must transform into final visual objects. |
| Dressing class | This is the real type of a dressing. Examples are:<br><ul><li class="">A simple texturing that applies a texture based on the face class</li><li class="">For opened containers: a rectangular space filled with boxes, a stair, a lift, ...</li></ul>The list of dressings can be augmented easily through new classes, this is explained in <span class="colour" style="color:rgb(212, 212, 212)">[</span><span class="colour" style="color:rgb(206, 145, 120)">3 Writing new mazes elements</span><span class="colour" style="color:rgb(212, 212, 212)">](<u>doc/3-writing\_maze\_elements.md</u>)</span> |
| Public parameters section | This is a list of main parameters that define the dressing, augmented with parameters forced by the designer |
| Private parameters section | This is the whole list of parameters that the dressing requires. Everything that is not in the public parameters is set randomly. This is defined in the next section. |
| Dressing Strategy | A class that allows to choose a dressing type from a list of characteristics.<br>A strategy can choose amongst a limited number of dressing types, or use the dressing types limitations to choose randomly.<br>The list of strategies can be aumgented through new classes. |

### Inputs

* Bricks.
* Dressing strategy that chooses dressing classes.

### Output

Every brick is given a dressing class with required limited public parameters.

## Dressing Personalization

### Rationale

The designer is given the ability to adapt the dressing class with specific "public" parameters. All those parameters are merged with default parameters into "private" parameters.

### Definitions

Public and private follows the same principles than for brick personalization.

### Inputs

* Dressing types
* Dressing public parameters

### Output

Dressing private parameters

<br>
<br>
