# Specifications

## Game (single player)

The player is given the ability to move in a maze made of (seemingly) closed rooms and move from room to room through gates.

A countdown is running to go to zero. The countdown starts at a given amount of time. Various events can add or remove time to the countdown.

The game's goal is to reach a given room, starting from another room, before the countdown reaches zero.

The player receives messages during the game to help him (...or deceive him):

* When entering a new room, the room's name is given. This helps the user "map" the maze.
* It is possible to have trigger send a message to the user.

## Generation

One major aspect of the project is to be able to easily generate new mazes so that the game is almost endless. This can be done at multiple levels. This is described in more detail in [1 Mazes generation process](1-generation.md).

## Maze description

Mazes are made of rooms. Each room is connected to others through gates. A gate consists in an architectural object with one face giving access to another room. This face is called a portal. Each room is euclidean by nature. The whole maze is not.

### Rooms

Each room has its own gravity rules, those can be dynamic and can contain sub-rules restricted to a given sub-space of the room.
Rooms contains gates that are "oriented": each face has its property.
Rooms contains objects, that can be bonuses or maluses. At the moment, only time bonuses/maluses are considered.
Rooms contains triggers that can change "events"

### Gates

Gates can connect two rooms from any space to any other space so can make the whole world (maze) non-euclidean, although they seem to be directly connected. Examples:

* can connect a room to a distant one (simple case)
* can shift axes in any way: go through a door that makes fall from the roof of
another room
* can be visible on one side and invisible on the other
* can be traversable on one side and not in the other direction

"Triggers" are elements of rooms that triggers changes in gates properties:
traversable, visibility.

### Gravity

Each room has gravity rules that can change dynamically.

Static examples:

* a rectangular room where each wall attracts
* a room like a planet, a cylinder, a donut, and so on.

Dynamic examples:

* A rectangular room where gravity flips at regular intervals

<br>
Rooms are made of sub-spaces which have their own gravity rules. Examples:

* Rooms made of stairs in every direction which have their own gravity. Directly inspired from Escher's paintings. See: [Escher's Relativity](https://en.wikipedia.org/wiki/File:Escher%27s_Relativity.jpg) .
* A rooms made of multiple planet-like spheres where the user can jump from one to another.

Unlike real physics, gravity depends on the objects weight, velocity, size.

### Portals

Portals are polygons showing another space in another room. They are
directional.
They can be active or inactive. They can move inside the source space.
When active, they show their destination and can be crossed. Destination is in
the space of the target room. In terms of organisation, there is no link between
original space and target space. For example, it is possible to have
Tardis-like (bigger inside than the outside) or Mario-like (smaller worlds in
bigger worlds) portals.

Portals can also change the scale of the objects they cross: bigger or smaller
by a given static factor. Crossing in the opposite direction changes the scale
in the opposite direction.

### Objects

Rooms can be filled with objects:

* time bonus, malus
* special effects
* gravity inverter option
* ...
* objects with changes in nature through time: health bonuses to/from mines.

### Triggers / Events

Rooms configurations can change with triggers.

Triggers can be:

* Visible / invisible.
* Voluntary / unvoluntary.

Examples:

* Switching a visible button is visible and voluntary, so-called "switch"
* Switching an invisible button is invisible and voluntary, so-called "secret"
* Crossing a visible line is visible and voluntary 
* Crossing an invisible line is invisible and voluntary

They set "events" to a given state which are binary.

Then some maze elements can react to events: this can animate some object (door, elevator, ...) or change gravity.

### Entry/Output

A maze for single player mode have an entry room that marks the spawn point
(middle of the room) and an output room that marks the end of the game.

## Maze structural aspects

This is done through "hints" provided to rooms generators.
Hints come in various categories, whether or not they apply are up to the room
class.

| object type | name | values | description |
| ----------- | ---- | ------ | ----------- |
| room | class | room, corridor, stairs, ...,<br>planet, ocean, hamster wheel,<br>bubble, escher painting, etc. | the structural identity of the room |
| room | ordered | ordered, disordered | whether rooms must have a general ordered aspect, with gates aligned, and so on |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |

.. list-table::
:header-rows: 1

* 
    * object type
    * name
    * values
    * description
* 
    * room
    * class
    * room, corridor, stairs, ...,
    planet, ocean, hamster wheel,
    bubble, escher painting, etc.
    * the structural identity of the room
* 
    * room
    * ordered
    * ordered, disordered
    * whether rooms must have a general ordered aspect, with gates aligned,
    and so on
* 
    * room
    * elements
    * blocks, rocks, swimming-pool, ...
    * provides additional elements to append, both visual and structural
* 
    * gate
    * class
    * window, door, openspace, water, hole
    * describes the type of gate
        * openspace creates the biggest link, like the 2 rooms would be only one.
        * water: a water surface
        * window: point of view, not traversable
        * door : something you would step in, with a given floor direction
        * hole: usually something you "falls into"
        * others are obvious
* 
    * room
    * minsize, maxsize
    * a set of 3 float values (3D vector)
    * The minimum or maximum size of a room in each direction. Although this
    may mean that the room is a cube, it is just a hint.
    A zero in one direction means there is not requirement.
    The unit is the meter. A player has a size of a 1.8m bubble (approx).
    Some room's types cannot be instantiated under a given size.
    The 3D vector refers to [X,Y,Z], Z being the expected top-bottom
    direction, at least for
* 
    * gate
    * minsize, maxsize
    * a set of 2 float values (2D vector)
    * The minimum or maximum size of a gate in each direction. Although this may mean
    that the room is a cube, it is just a hint.
    A zero in one direction means there is not requirement.
    The unit is the meter. A player has a size of a 1.8m bubble (approx).
    Some gate's types cannot be instantiated under a given size.
    The 2D vector refers to [width, height]

## Maze visual aspects

This is done through "hints" provided to rooms "dressing"s. Each dressing class
can be modified by a list of parameters. Hints come in various categories:

.. list-table::
:header-rows: 1

* 
    * object type
    * name
    * values
    * description
* 
    * gate, room
    * class
    * futuristic, mine, hangar...
    * Global visual style. Choose with care to have a stylish view
* 
    * gate, room
    * texture
    * futuristic, mine, ...
    * Global visual texturing. Choose with care to have a stylish view