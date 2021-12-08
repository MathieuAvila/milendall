
Maze generation
===============

This section goes more in depth with maze generation. In this section,
"designer" refers to a human who controls the creation process.

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
     - JSON file of strategy
   * - Level Steps
     - Description of steps that happen: connection between steps,
       what they contains. Those steps describes the main events happening in a
       game, like opening a door and giving time points.
     - JSON file of game play

       Viz thru Plantuml
   * - Rooms descriptions
     - Descriptions of rooms & gates : how they connect, their configurations,
       their high-level content. May include hints on decoration, room type,
       etc...

       This is the global working of the maze.
     - JSON file of level

       Viz thru Plantuml
   * - Rooms instantiation
     - Force structure and dressing of rooms not described in previous step.

       Example: room type, size, colors...

       Using this step to fine-tune content. All parameters not chosen by the
       designer is set here.

       Normally once this step is done it must generate the exact same maze in
       the next steps.
     - JSON file

       Viz thru Plantuml
   * - Rooms structure
     - Generate rooms structural elements: hard walls structure.

       At this point, walls position and sizes are generated, but not yet walls
       graphical structure and texturing.

       Gravity rules are computed.

       Objects are placed.

       It is still possible to re-compute one's room structural make-up without
       affecting everything.

       Walls come as a list of polygons that describe the main structure. Gates
       are computed as a "punch" in one of the wall.

     - Set of rooms and gates with list of structural polygons and gates. This
       starts to be navigable, although it's ugly and uniform. glTF format.
   * - Rooms dressing
     - Generate all graphical elements of rooms

       After this, it is possible to re-compute a graphical setup if it is not
       satisfying.

       Otherwise, this is finalized.

       Walls are cut into real final polygons, based on previous main structure.
       Main structure can still used to check that you don't go through terrain,
       or this can be done with visual polygons. It's up to the specified
       dresser algorithm.

     - Playable level. glTF format.

Note that designer is given the ability to finely control every generation step
through parameters. However, it is still possible to let the system handle all
details with random generation.

Level strategy
--------------

TBD

Level steps
-----------

TBD

Rooms Description
-----------------

This is highest level of description of rooms.

A list of rooms is given, along with the lists of portals that allows to go
from one room to another. The designer has the ability to fix a few parameters
in this stage.

Rooms Instantiation
-------------------



Rooms Structure
---------------

TBD

Rooms Dressing
-----------------

TBD
