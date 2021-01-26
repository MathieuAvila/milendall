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
Each room has a list of “configurations”. Each configuration has a subset of the original objects and portals.

Rooms have both:

- a structural aspect, as they give bounds to objects
- a graphical aspect, as they provide a visual rendering

Portals
.......

Portals are polygons showing another space in another room. They are directional.
They can be active or inactive. They can move inside the source space.
When active, they show their destination and can be crossed. Destination is in the
space of the target room. There is no link between original space and target space.

Gates
.....

Gates are both:

- decorations around portals
- structural elements inside rooms.

Gates are the concrete elements that links 2 rooms, and as such they force portal
polygon geometry, orientation (where should gravity happen) and decorations inside rooms. 

Gravity
.......

Gravity is defined on a per-room basis.

Maze visual aspect
------------------

This is done through "hints" provided to rooms generators.
Hints come in various categories

.. list-table::
   :header-rows: 1

   * - object type
     - name
     - values
     - description     
   * - room
     - ordered
     - ordered, disordered
     - whether rooms must have a general ordered aspect, with gates aligned, and so on
   * - room
     - elements
     - blocks, rocks, swimming-pool
     - provides additional elements to append, both visual and structural
   * - gate
     - type
     - window, door, openspace, water
     - describes the type of gate
     
       - openspace creates the biggest link, like the 2 rooms would be only one.
       - water: a water surface
       - others are obvious
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
     - Descriptions of rooms & gates : how they connect, their configurations, their high-level content. May include hints on decoration, room type, etc...
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




