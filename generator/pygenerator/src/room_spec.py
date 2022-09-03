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

class WriteableString():
    def __init__(self):
        self.content = ""

    def write(self, string):
        self.content = self.content + string

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
        dump a graphviz repr of a room, only spec part
        """

        logger.info("Dump room_spec %s" % (self.values.name))

        # collect portals and write them out
        gate_ids = []
        for brick in self.room.values["bricks"]:
            if "portals" in brick.values:
                for portal in brick.values.portals:
                    gate_ids.append(portal.gate_id)
        for g in gate_ids:
            output.write('"' + g + '" [ shape=hexagon style=filled color=yellow ];\n')

        # now you can dump the room
        label = self.values.room_id
        if self.values.triggers is not None:
            label += "<BR/><B>".join(["\n"] + [t.trigger_id for t in self.values.triggers])+"</B>"
        if self.values.structure_class is not None:
            label += "<BR/><I>S: "+ self.values.structure_class + "</I>"
        if self.values.dressing_class is not None:
            label += "<BR/><I>D: "+ self.values.dressing_class + "</I>"
        output.write('subgraph "cluster_' + self.values.room_id +'" {\n')

        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)

        output_room = WriteableString()
        output_main = WriteableString()

        logger.info("Go to dump room %s" % (self.values.name))
        self.room.dump_graph(output_room, output_main)

        output.write(output_room.content)

        output.write('style=filled;\n')
        output.write('color=lightgrey;\n')
        output.write('label=< ' + label+ ' >;\n')
        output.write('};\n')

        output.write(output_main.content)


    def structure_personalization(self):
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(state.LevelState.Instantiated)
        self.room.structure_personalization()
        self.state = state.LevelState.Personalized

    def dressing_instantiation(self):
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(state.LevelState.Personalized)
        self.room.dressing_instantiation()
        self.state = state.LevelState.DressingInstantiated

    def dressing_personalization(self):
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(state.LevelState.DressingInstantiated)
        self.room.dressing_personalization()
        self.state = state.LevelState.DressingPersonalized

    def finalize(self, output_directory, preview=False):
        """Run the final process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(state.LevelState.DressingPersonalized)
        self.room.finalize(output_directory, preview)

    def save(self, output_directory):
        if self.room is not None:
            self.room.save(output_directory)
