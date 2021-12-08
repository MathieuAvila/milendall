Milendall
==========

Milendall aims to be an escape first-person view game in a maze with
non-euclidean geometry and gravity rules. It can be played in single or
multi-player modes.

Mazes can be generated randomly, with control on the randomness aspects at
various levels, to make them interesting from a player's point-of-view. A set of
pre-computed mazes (levels) are available.

Pitch
-----

Gwenaëlle, a 12 year old girl, is making a dream in which she is playing to get
out of a maze, and she has a only a few minutes left to escape before she
awakes. However, this is a wonderful dream with lots of wonder (unicorns and
alikes), so that she isn't scared at all.

Game Goal
---------

In single player mode:

- Get out of a maze in a given amount of time.

In multi-player mode:

- TBD

Maze characteristics
--------------------

In both cases:

- Maze is globally non-euclidean at the maze scale, but euclidean at local scale
  (in spaces called rooms)
- Mazes are bound (not open worlds), but can loop in bizarre ways
- Gravity is dynamic and unusual
- Objects can be rescaled when passing through some gates
- There are some objects left that gives time bonuses / maluses
- Objects can change in nature, contrary to most FPS (time bonuses become time
  maluses, and so on...)
- There are triggers that can be volontarily activated and others are
  unvolontary. Those triggers can move objects (open doors, move lifts, ...)
- Messages are sent to the player: room's name when entering one, subtle
  meaningful messages or useless ones,...

Organization
============

This doc is organized as follows:

- [0 Game specification](doc/0-specification.md)
- [1 Mazes generation process](doc/1-generation.md)
- [2 Writing new mazes](doc/2-writing_new_mazes.md)
- [3 Writing new mazes elements](doc/3-writing_maze_elements.md)
- [4 Game engine](doc/4-engine.md)
