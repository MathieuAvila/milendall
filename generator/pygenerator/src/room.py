"""
Definition of room. Not to be confused with room_spec (RoomSpec)
"""

import brick
import concrete_room
import pathlib
from element import Element
import json_helper
import state
import logging

from munch import DefaultMunch

logger = logging.getLogger("room")
logger.setLevel(logging.INFO)

class Room(Element):

    filename_map = {
        state.LevelState.Instantiated : "room-instantiated.json",
        state.LevelState.Personalized : "room-personalized.json",
        state.LevelState.DressingInstantiated : "room-dressing-instantiated.json",
        state.LevelState.DressingPersonalized : "room-dressing-personalized.json",
    }

    def __init__(self, level_directory, name, selector):
        '''Initialize a Room to defaults'''
        self.name = name
        self.state = state.LevelState.New
        self.level_directory = level_directory
        self.selector = selector
        values = {
                "bricks":[]
            }
        self.values = DefaultMunch.fromDict(values)

    def decode_room(self):
        """automatically set object type"""
        def _decode_room(dct):
            if 'b_id' in dct:
                return brick.Brick(dct, self.selector)
            return dct
        return _decode_room

    def load(self, state):
        self.state = state
        logger.info("Load room %s from: %s, state: %s" % (self.name, self.level_directory, self.state))
        file_name = self.level_directory + "/" + self.name + "/" + self.filename_map[state]
        logger.info("Load room from: %s, state: %s" % (file_name, self.state))
        values = json_helper.load_and_validate_json(
                file_name,
                "file_room_bricks.json",
                decode_hook=self.decode_room())
        logger.info("Has read: %s", values)
        self.values = DefaultMunch.fromDict(values)
        logger.info("Has values: %s", self.values)

    def add_brick(self, brick):
        self.values["bricks"].append(brick)

    def structure_personalization(self):
        '''Instantiate everything'''
        self.state = state.LevelState.Personalized
        for brick in self.values["bricks"]:
            brick.structure_personalization()

    def dressing_instantiation(self):
        '''Instantiate everything'''
        self.state = state.LevelState.DressingInstantiated
        for brick in self.values["bricks"]:
            brick.dressing_instantiation()

    def dressing_personalization(self):
        '''Instantiate everything'''
        self.state = state.LevelState.DressingPersonalized
        for brick in self.values["bricks"]:
            brick.dressing_personalization()

    def save(self, level_directory=None):
        '''Save to file. File depends on instantiated or not'''
        if level_directory == None:
            level_directory = self.level_directory
        file_name = self.filename_map[self.state]

        room_path = level_directory + "/" + self.name
        room_file = room_path + "/" + file_name
        pathlib.Path(room_path).mkdir(parents=True, exist_ok=True)
        _j = json_helper.dump_json(self)
        with open(room_file, "w") as output_file:
            output_file.write(_j)

    def dump_graph(self, output_room, output_main):
        """
        dump a graphviz repr of a room
        """

        label = self.name
        logger.info("Dump room %s" % (self.name))
        for brick in self.values["bricks"]:
            logger.info("Dump brick %s" % (self.name))
            brick.dump_graph(output_room, output_main, self.name)
        #output.write('"' + self.name +'" ' + '[ label=< ' + label+ ' > ] ;\n')

    def finalize(self, level_directory=None, preview=False):
        """ Perform final generate and dressing on one room."""

        concrete = concrete_room.ConcreteRoom()

        for brick in self.values["bricks"]:
            logger.info("in brick %s" % brick.values.b_id)
            concrete_brick = concrete_room.ConcreteRoom()
            brick.finalize(concrete_brick)
            concrete_brick.append_prefix(brick.values.b_id + "_")
            if "root_pad" in brick.values:
                root_id = brick.values.root_pad.ref_b_id + "_" + brick.values.root_pad.ref_pad_id
                logger.info("set root pad to: %s", root_id)
                concrete_brick.set_root(root_id)
            concrete.merge(concrete_brick)

        #for gate in self.gates:
        #    if not hasattr(gate, "concrete"):
        #        gate.concrete = concrete_room.ConcreteRoom()
        #        gate.structure.generate(gate.concrete)
        #        gate.dressing.generate(gate.concrete)
        #    concrete.merge(gate.concrete)

        if level_directory == None:
            level_directory = self.level_directory
        room_path = level_directory + "/" + self.name
        pathlib.Path(room_path).mkdir(parents=True, exist_ok=True)
        concrete.generate_gltf(room_path)
        if preview:
            concrete_room.preview(room_path + "/room.gltf", room_path + "/room_preview.gltf")
