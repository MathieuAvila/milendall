Incoherent
==========

Incoherent is a FPS-like game in a voluntarily incoherent maze. It can be played in single- or multi-player modes.

Mazes can be generated randomly, with various control on the randomness to make them interesting from
a game point-of-view. A set of pre-computed mazes (levels) are available.

Game Goal
---------

In single player mode:

- Get out of maze in a given amount of time.

In multi-player mode:

- Death match in a maze

Particularity
-------------

In both cases:

- Maze is non-euclidian
- Gravity is dynamic and incoherent
- Objects can be rescaled when passing through some gates
- FPS classics: Enemies get on the way. Can be fought with weapons & ammo

Requirements
============

Maze description
----------------

Maze is made of rooms. Each room is connected to others through portals.

Rooms
.....

Each room has gravity rules that can change dynamically. Ex: a room where each wall attracts, a room like a planet.
Each room has a fixed list of portals
Each room has a list of “configurations”. Each configuration has a subset of the original objects and portals.

Rooms have both:

- a structural aspect, as they give bounds to objects
- a graphical aspect, as they provide a visual rendering

Portals
.......

Portals are polygons showing another space in another room. They are directional.
They can be active or inactive. They can move inside the source space.
When active, they show their destination and can be crossed. Destination is in the
space of the target room. In terms of organisation, there is no link between 
original space and target space. For example, it is possible to have
Tardis-like (bigger inside than the outise) or Mario-like (smaller worlds in bigger worlds)
portals.

Portals can also change the same of the objects they cross: bigger or smaller by a 
given static factor. Crossing in the opposite direction changes the scale in the
opposite direction.

Gates
.....

Gates are both:

- decorations around portals
- structural elements inside rooms.

Gates are the concrete elements that links 2 rooms, and as such they force portal
polygon geometry, orientation (where should gravity happen) and decorations inside rooms. 

Gravity
.......

Gravity is defined on a per-room basis. Default will be the expected one: down on Z,
value 1.0, constant in space and time.

It can be variable in space and time. It is customizable.


Objects
.......

Rooms can be filled with objects:

- standard FPS ones:

  - enemies
  - health
  - armor
  - ammo
  - special effects (mega-health, ...)

- unstandard ones:

  - gravity inverter option
  - weapon effect inverter option
  - ...

Events
......

Rooms configurations can change with triggers:

- buttons placed in other rooms.
- "keys" with a given color.

For the sake of the player (or not), it can be devised which effect they have.

Events are customizable through scripts. However here are some possible changes:

- obviously, opening of closing gates
- gravity change (see gravity)
- appearance/disappearance of structural elements (see mesh animation)

Entry/Output
............

A maze for single player mode have an entry room that marks the start (middle of it) and an output room
that marks the end of the game.

A maze for multi-player mode have many entry rooms and no output room.


Maze structural aspects
-----------------------

This is done through "hints" provided to rooms generators.
Hints come in various categories

.. list-table::
   :header-rows: 1

   * - object type
     - name
     - values
     - description     
   * - room
     - type
     - room, corridor, stairs, ...,
     
       planet, ocean, hamster wheel,
       
       bubble, escher painting, etc.
     - the structural identity of the room
   * - room
     - ordered
     - ordered, disordered
     - whether rooms must have a general ordered aspect, with gates aligned, and so on
   * - room
     - elements
     - blocks, rocks, swimming-pool, ...
     - provides additional elements to append, both visual and structural
   * - gate
     - type
     - window, door, openspace, water
     - describes the type of gate
     
       - openspace creates the biggest link, like the 2 rooms would be only one.
       - water: a water surface
       - others are obvious
   * - room
     - minsize, maxsize
     - a set of 3 float values (3D vector)
     - The minimum or maximum size of a room in each direction. Although this may mean
       that the room is a cube, it is just a hint.

       A zero in one direction means there is not requirement.

       The unit is the meter. A player has a size of a 1.8m bubble (approx).

       Some room's types cannot be instantiated under a given size.

       The 3D vector refers to [X,Y,Z], Z being the expected top-bottom direction, at least
       for  
   * - gate
     - minsize, maxsize
     - a set of 2 float values (2D vector)
     - The minimum or maximum size of a gate in each direction. Although this may mean
       that the room is a cube, it is just a hint.

       A zero in one direction means there is not requirement.

       The unit is the meter. A player has a size of a 1.8m bubble (approx).

       Some gate's types cannot be instantiated under a given size.

       The 2D vector refers to [width, height]

Maze visual aspects
-------------------

This is done through "hints" provided to rooms dressers.
Hints come in various categories

.. list-table::
   :header-rows: 1

   * - object type
     - name
     - values
     - description
   * - gate, room
     - decoration
     - futuristic, mine, hangar...
     - Global visual style. Choose with care to have a stylish view
   * - gate, room
     - texture
     - futuristic, mine, ...
     - Global visual texturing. Choose with care to have a stylish view

Maze generation
===============

This section goes more in depth with maze generation. In this section, "designer" refers to
a human who controls the creation process.

Overview 
--------

Generation happens with these steps:

.. list-table::
   :header-rows: 1

   * - Step name 
     - Step description 
     - Output
   * - Level Strategy
     - Highest-level view which gives difficulty, ambiance, rythm
     - JSON file
   * - Rooms descriptions
     - Descriptions of rooms & gates : how they connect, their configurations,
       their high-level content. May include hints on decoration, room type, etc...
     - JSON file

       Viz thru Plantuml
   * - Rooms instantiation
     - Force description of rooms not described in previous step.
     
       Example: room type, size, colors...
       
       Using this step to fine-tune content
     - JSON file

       Viz thru Plantuml
   * - Rooms structure
     - Generate rooms structural elements: hard walls structure.

       At this point, walls position and sizes are decided, but not yet walls
       graphical structure and texturing.

       Gravity rules are computed.

       Objects are placed.

       It is still possible to re-compute one's room structural make-up without
       affecting everything.

       Walls come as a list of polygons that describe the main structure. Gates
       are computed as a "punch" in one of the wall.

     - Set of rooms and gates with list of structural polygons and gates. This starts to be navigable,
       although it's ugly and uniform.
   * - Rooms dressing
     - Generate all graphical elements of rooms
     
       After this, it is possible to re-compute a graphical setup if it is not satisfying.
       
       Otherwise, this is finalized.

       Walls are cut into real final polygons, based on previous main structure. Main structure
       can still used to check that you don't go through terrain, or this  can be done with
       visual polygons. It's up to the specified dresser algorithm.

     - Playable level

Note that designer is given the ability to finely control every generation step.
However, it is still possible to let the system handle all details.

Level strategy
--------------

TBD

Rooms Description
-----------------

This is highest level of description of rooms.

A list of rooms is given, along with the lists of portals that allows to go from one room to another. 
The designer has the ability to fix a few parameters in this stage.

Rooms Instantiation
-------------------



Rooms Structure
---------------

TBD


Rooms Dressing
-----------------

TBD
