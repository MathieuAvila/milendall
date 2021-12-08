Requirements
============

Maze description
----------------

Mazes are made of rooms. Each room is connected to others through gates. A gate
consists in an architectural object with one face giving access to another room.
This face is called a portal. Each room is euclidean by nature. Each room has
its own gravity rules, those can be dynamic.

Objects that pass through gates can be rescaled.
Gates can connect two rooms from any space to any other space so can make the
whole world (maze) non-euclidean, although they seem to be directly connected.
Examples:

- can connect a room to a distant one (simple case)
- can shift axes in any way: go through a door that makes fall from the roof of
  another room
- can be symmetrical: you enter where you leave, like a mirror
- can connect the same room to itself
- can be visible on one side and invisible on the other
- can be traversable on one side and not in the other direction

"Triggers" are elements of rooms that triggers changes in gates properties:
traversable, visibility.

This is a run against time. There is no ennemy as such, but the maze must be
solved before the counter reaches zero. A given amount of time is given at the
start of the game.

Rooms
.....

Each room has gravity rules that can change dynamically. Ex: a room where each
wall attracts, a room like a planet, a cylinder, a donut, and so on. Imagination
is the only limit. Each room has a fixed list of gates.

Rooms have both:

- a structural aspect, as they give bounds to objects
- a graphical aspect, as they provide a visual rendering

Other properties:

- gravity rules, based on objects types, location, velocity, weight
  (but does this value has any sense at all ?).
- enclosed objects: bonuses, maluses, help
- triggers: activable by user to change gates properties

Portals
.......

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

Gates
.....

Gates are both:

- decorations around portals
- structural elements inside rooms.

Gates are the concrete elements that links 2 rooms, and as such they force
portal polygon geometry, orientation (where should gravity happen) and
decorations inside rooms.

Gravity
.......

Gravity is defined on a per-room basis. Default will be the expected one: down
on Y, value 1.0, constant in space and time.

It can be variable in space and time. It is customizable.

Objects
.......

Rooms can be filled with objects:

  - time bonus, malus
  - special effects
  - gravity inverter option
  - ...
  - objects with changes in nature through time: health bonuses to/from mines.

Events
......

Rooms configurations can change with triggers which appears as buttons placed
anywhere.

For the sake of the player (or not), it can be devised which effect they have.

Events are customizable through scripts. However here are some possible changes:

- obviously, opening of closing gates
- gravity change (see gravity)
- appearance/disappearance of structural elements (see mesh animation)

Entry/Output
............

A maze for single player mode have an entry room that marks the spawn point
(middle of the room) and an output room that marks the end of the game.

**TBD** A maze for multi-player mode have many entry rooms and no output room.

Maze structural aspects
-----------------------

This is done through "hints" provided to rooms generators.
Hints come in various categories, whether or not they apply are up to the room
class.

.. list-table::
   :header-rows: 1

   * - object type
     - name
     - values
     - description
   * - room
     - class
     - room, corridor, stairs, ...,

       planet, ocean, hamster wheel,

       bubble, escher painting, etc.
     - the structural identity of the room
   * - room
     - ordered
     - ordered, disordered
     - whether rooms must have a general ordered aspect, with gates aligned,
       and so on
   * - room
     - elements
     - blocks, rocks, swimming-pool, ...
     - provides additional elements to append, both visual and structural
   * - gate
     - class
     - window, door, openspace, water, hole
     - describes the type of gate

       - openspace creates the biggest link, like the 2 rooms would be only one.
       - water: a water surface
       - window: point of view, not traversable
       - door : something you would step in, with a given floor direction
       - hole: usually something you "falls into"
       - others are obvious
   * - room
     - minsize, maxsize
     - a set of 3 float values (3D vector)
     - The minimum or maximum size of a room in each direction. Although this
       may mean that the room is a cube, it is just a hint.

       A zero in one direction means there is not requirement.

       The unit is the meter. A player has a size of a 1.8m bubble (approx).

       Some room's types cannot be instantiated under a given size.

       The 3D vector refers to [X,Y,Z], Z being the expected top-bottom
       direction, at least for
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

This is done through "hints" provided to rooms "dressing"s. Each dressing class
can be modified by a list of parameters. Hints come in various categories:

.. list-table::
   :header-rows: 1

   * - object type
     - name
     - values
     - description
   * - gate, room
     - class
     - futuristic, mine, hangar...
     - Global visual style. Choose with care to have a stylish view
   * - gate, room
     - texture
     - futuristic, mine, ...
     - Global visual texturing. Choose with care to have a stylish view
