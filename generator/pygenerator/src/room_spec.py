"""
Definition of a RoomSpec.
Those are the high-level definition embedded in rooms_logic, and contains
Containers definitions
Do not confuse them with Room element which are the containers for bricks.
"""

from munch import DefaultMunch
import room
import state

import logging

logger = logging.getLogger("room_spec")
logger.setLevel(logging.INFO)

class RoomSpec():

    def __init__(self, values, level_directory, state, selector):
        self.values = DefaultMunch.fromDict(values)
        self.level_directory = level_directory
        # this is the reference to the Room in its directory.
        # It is instantiated when needed.
        self.state = state
        self.room = None
        self.selector = selector
        logger.info("New room_spec %s from %s" % (self.values.room_id, self.level_directory) )

    def dump_graph(self, output):
        """
        dump a graphviz repr of a room
        """

        label = self.values.room_id
        if self.values.triggers is not None:
            label += "<BR/><B>".join(["\n"] + [t.trigger_id for t in self.values.triggers])+"</B>"
        if self.values.structure_class is not None:
            label += "<BR/><I>S: "+ self.values.structure_class + "</I>"
        if self.values.dressing_class is not None:
            label += "<BR/><I>D: "+ self.values.dressing_class + "</I>"
        output.write('"' + self.values.room_id +'" ' + '[ label=< ' + label+ ' > ] ;\n')

    def structure_personalization(self):
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(state.LevelState.Instantiated)
        self.room.structure_personalization()

    def dressing_instantiation(self):
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(state.LevelState.Personalized)
        self.room.dressing_instantiation()

    def dressing_personalization(self):
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(state.LevelState.DressingInstantiated)
        self.room.dressing_personalization()

    def finalize(self, output_directory, preview=False):
        """Run the final process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(state.LevelState.DressingPersonalized)
        self.room.finalize(output_directory, preview)

