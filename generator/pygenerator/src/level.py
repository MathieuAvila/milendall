""" A level definition"""

import logging
import pathlib
import os
from enum import Enum

import json
import json_helper

import room_spec
import gate
import concrete_room
import state

from munch import DefaultMunch

logger = logging.getLogger("level")
logger.setLevel(logging.INFO)

def decode_level(level_directory, state, selector):
    """automatically set object type"""
    def _decode_level(dct):

        if 'room_id' in dct:
            return room_spec.RoomSpec(dct, level_directory, state, selector)
        if 'gate_id' in dct:
            return gate.Gate(dct)
        return dct

    return _decode_level

class Level:

    FILENAME_INSTANTIATED = "level-instantiated.json"
    FILENAME_PERSONALIZED = "level-personalized.json"

    status_to_filename = {
        state.LevelState.Personalized : FILENAME_PERSONALIZED,
        state.LevelState.Instantiated : FILENAME_INSTANTIATED,
    }

    def __init__(self, _selector):

        self.status = state.LevelState.New
        self.selector = _selector
        self.state = state.StateList()

    def load(self, directory, load_state):
        self.directory = directory
        self.state = state.StateList(directory + "/state.txt")
        if not self.state.has_state(load_state):
            raise Exception("Level has no saved state %s" % load_state)
        logger.info(load_state in self.status_to_filename.keys())
        logger.info(self.status_to_filename[load_state])
        obj = json_helper.load_and_validate_json(
            directory + "/" + self.status_to_filename[load_state],
            "file_rooms_logic.json",
            decode_hook=decode_level(directory, self.status, self.selector))
        self.status = load_state
        self.values = DefaultMunch.fromDict(obj)
        self.structure_check_coherency()

    def save(self, directory):
        """save to a file"""
        if self.status == state.LevelState.New:
            raise Exception("Unable to save a new level.")
        else:
            filename = directory + "/" + self.status_to_filename[self.status]
        with open(filename, "w") as output_file:
            _j = json_helper.dump_json(self)
            output_file.write(_j)
        self.state.save(directory + "/state.txt")

    def dump_graph(self, filename):
        """ Dump a file in graphviz format that allows to graphically visualize
            level"""
        with open(filename, "w") as output_file:
            output_file.write("digraph g {\n")
            output_file.write("node [shape=box];\n")
            for _r in self.values.rooms:
                _r.dump_graph(output_file)
            for _g in self.values.gates:
                _g.dump_graph(output_file)
            output_file.write("}\n")

    def _check_parameter_presence(self, element, structure_type, parameter_name):
        """Check a parameter is present"""
        logger.info("Check %s: %s", structure_type, element.values.gate_id)
        if element.values[parameter_name] is None:
            raise Exception ("%s has no %s parameter." % (structure_type, parameter_name))

    def get_room(self, _name):
        """ return room object from its name, otherwise raise exception"""
        room_list = [r for r in self.values.rooms if r.values.room_id == _name ]
        if len(room_list) != 1:
            raise Exception ("More than one room with ID=%s" % _name)
        return room_list[0]

    def structure_check_coherency(self):
        """ Sanity check that content is viable, at the structure level
            Thing can get insane if user has messed up with content in-between

            1. For each room, if room type is selected
            2. gather the gate lists with their format
            3. Check it is allowed by the room type
            4. Request room structure coherency"""
        logger.debug("Structure check coherency")
        for _gate in self.values.gates:
            logger.debug("Check gate: %s", _gate.values.gate_id)

            # TODO check gates appear in exactly 2 rooms

            if _gate.values.structure_class is not None:
                _gate.structure = self.selector.get_structure_from_name(
                    _gate.values.structure_class,
                    _gate)
                _gate.structure.check_structure()

        for _room in self.values.rooms:
            logger.debug("Check room: %s", _room.values.room_id)
            if _room.values.structure_class is not None:
                _room.structure = self.selector.get_structure_from_name(
                    _room.values.structure_class,
                    _room)
                _room.structure.check_structure()

    def dressing_check_coherency(self):
        """ Sanity check that content is viable, at the structure level
            Thing can get insane if user has messed up with content in-between"""
        logger.info("Dressing check coherency")
        for _gate in self.values.gates:
            logger.debug("Check gate: %s", _gate.values.gate_id)

            if _gate.values.dressing_class is not None:
                _gate.dressing = self.selector.get_dressing_from_name(
                    _gate.values.dressing_class,
                    _gate)

        for _room in self.values.rooms:
            logger.debug("Check room: %s", _room.values.room_id)
            if _room.values.dressing_class is not None:
                _room.dressing = self.selector.get_dressing_from_name(
                    _room.values.dressing_class,
                    _room)

    def _element_personalization(self, _element):
        _element.structure_personalization()

    def structure_personalization(self):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.gates + self.values.rooms:
            self._element_personalization(_element)

    def element_personalization(self, _id):
        """ wrapper to _element_instantiation, with ID name resolved"""
        element = None
        self._element_personalization(element)

    def objects(self):
        """ Place objects in room acording to specs"""

    def room_objects(self, room_id):
        """ Place objects in one room acording to specs"""

    def dressing_instantiation(self):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.gates + self.values.rooms:
            _element.dressing_instantiation()

    def dressing_personalization(self):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.gates + self.values.rooms:
            _element.dressing_personalization()

    def finalize(self, output_directory, preview=False):
        """ Finalize level, generate final output data"""
        self.dressing_check_coherency()
        pathlib.Path(output_directory).mkdir(parents=True, exist_ok=True)
        level_file = output_directory + "/level.json"
        schema = os.path.realpath(os.getcwd() + "/../../schema/final.level.schema.json")
        level_content = {
                "$schema" : schema,
                "version": "1.0",
                "rooms" : [],
                "section" : "",
                "game_type" : {}
            }
        level_content["game_type"] = self.values.game_type
        level_content["section"] = self.values.section
        for room in self.values.rooms:
            level_content["rooms"].append({"room_id": room.values.room_id, "name": room.values.name})
            room.finalize(output_directory, preview)
        level_json = json.dumps(level_content, indent=1)
        with open(level_file, "w") as output_file:
            output_file.write(level_json)
