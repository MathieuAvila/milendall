incoherent
==========

- Get out of maze in a given amount of time.
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
Each room has a list of “configurations”. Each configuration has a subset of the original objects and portals

Portals
.......

Portals are polygonal fragments showing another space in another room. They are directional.
They can be active or inactive. They can move inside the source space.
When active, they show their destination and can be crossed. Destination is in the space of the target room. There is no link between original space and target space

Gravity
.......

Gravity is defined on a per-room basis.

Maze generation
===============

Generation happens with these steps:

.. list-table::
   :header-rows: 1

   * - Step name 
     - Step description 
     - Output
   * - Level Strategy
     - Highest-level view which gives difficulty, ambiance, rythm
     - XML file
   * - Rooms descriptions
     - Highest-level view which gives difficulty, ambiance, rythDescriptions of rooms & gates : how they connect, their configurations, their high-level content. May include hints on decoration, room type, etc...
     - XML file

       Viz thru Plantuml
   * - Rooms instantiation
     - Force description of rooms not described in previous step.
      
       Example: room type, size, colors...
       
       Using this step to fine-tune content
     - XML file

       Viz thru Plantuml
   * - Rooms finalization
     - Generate rooms
     - Playable level

Level strategy
--------------

TBD

Rooms description & instantiation format
----------------------------------------




