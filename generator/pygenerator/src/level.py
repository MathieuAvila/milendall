""" A level definition"""

import logging
import pathlib
import os
from enum import Enum

import json
import json_helper

import room_spec
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
        #if 'gate_id' in dct:
        #    return gate.Gate(dct, level_directory, state, selector)
        return dct

    return _decode_level

class Level:

    FILENAME_INSTANTIATED = "level-instantiated.json"
    FILENAME_PERSONALIZED = "level-personalized.json"

    status_to_filename = {
        state.LevelState.Personalized : FILENAME_PERSONALIZED,
        state.LevelState.Instantiated : FILENAME_INSTANTIATED,
        state.LevelState.DressingInstantiated : FILENAME_INSTANTIATED,
        state.LevelState.DressingPersonalized : FILENAME_INSTANTIATED,
    }

    DUMP_INSTANTIATED = "dump-instantiated.graph"
    DUMP_PERSONALIZED = "dump-personalized.graph"

    status_to_dump_graph = {
        state.LevelState.Instantiated : DUMP_INSTANTIATED,
        state.LevelState.Personalized : DUMP_PERSONALIZED,
        state.LevelState.DressingInstantiated : DUMP_PERSONALIZED,
        state.LevelState.DressingPersonalized : DUMP_PERSONALIZED,
    }

    def __init__(self, _selector):

        self.status = state.LevelState.New
        self.selector = _selector
        self.state = state.StateList()

    def read_state_list(self, directory):
        return state.StateList(directory + "/state.txt")

    def load(self, directory, load_state):
        self.directory = directory
        self.state = state.StateList(directory + "/state.txt")
        logger.info("Saved states are: %s" % self.state)
        if not self.state.has_state(load_state):
            raise Exception("Level has no saved state %s" % load_state)
        logger.info(self.status_to_filename[load_state])
        obj = json_helper.load_and_validate_json(
            directory + "/" + self.status_to_filename[load_state],
            "file_final_level.json",
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
        pathlib.Path(directory).mkdir(parents=True, exist_ok=True)
        with open(filename, "w") as output_file:
            _j = json_helper.dump_json(self)
            output_file.write(_j)
        for _room in self.values.rooms:
            _room.save(directory)
        self.state.save(directory + "/state.txt")

    def dump_graph(self, output_dir):
        """ Dump a file in graphviz format that allows to graphically visualize
            level"""
        output_file_name = output_dir + "/" + self.status_to_dump_graph[self.status]
        with open(output_file_name, "w") as output_file:
            output_file.write("digraph g {\n")
            output_file.write('newrank="true"\n');
            output_file.write("node [shape=box];\n")
            for _r in self.values.rooms:
                _r.dump_graph(output_file)
            output_file.write("}\n")
        value = os.system("dot -Tpng " + output_file_name + " -o " + output_file_name + ".png")
        if value != 0:
            logger.error("Graph generation returned: %i" % value)


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
        for _room in self.values.rooms:
            logger.debug("Check room: %s", _room.values.room_id)
            if _room.values.dressing_class is not None:
                _room.dressing = self.selector.get_dressing_from_name(
                    _room.values.dressing_class,
                    _room)

    def structure_personalization(self):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.rooms:
            _element.structure_personalization()
        self.state.add_state(state.LevelState.Personalized)
        self.status = state.LevelState.Personalized

    def objects(self):
        """ Place objects in room acording to specs"""

    def room_objects(self, room_id):
        """ Place objects in one room acording to specs"""

    def dressing_instantiation(self):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.rooms:
            _element.dressing_instantiation()
        self.state.add_state(state.LevelState.DressingInstantiated)
        self.status = state.LevelState.DressingInstantiated

    def dressing_personalization(self):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.rooms:
            _element.dressing_personalization()
        self.state.add_state(state.LevelState.DressingPersonalized)
        self.status = state.LevelState.DressingPersonalized

    def run_step(self, to_state):
        if to_state != self.status + 1:
            raise("Invalid state to run from %s" % self.status)
        if to_state == state.LevelState.Personalized:
            self.structure_personalization()
        elif to_state == state.LevelState.DressingInstantiated:
            self.dressing_instantiation()
        elif to_state == state.LevelState.DressingPersonalized:
            self.dressing_personalization()
        elif to_state == state.LevelState.Finalize:
            pass
        else:
            raise("Don't know how to reach state: %s" % to_state)

    def preview(self, output_directory):
        if self.status == state.LevelState.Finalize:
            self.finalize(self, output_directory, True)
        else:
            self.dump_graph(output_directory)

    def finalize(self, output_directory, preview=False):
        """ Finalize level, generate final output data"""
        self.dressing_check_coherency()
        pathlib.Path(output_directory).mkdir(parents=True, exist_ok=True)
        level_file = output_directory + "/level.json"
        schema = os.path.realpath(os.getcwd() + "/../../schema/file_final_level.json")
        level_content = {
            "$schema" : schema,
            "version": "1.0",
            "rooms" : [],
            "declarations": {
                "section": "",
                "game_type": {
                    "single_mode": {
                        "end_point_room": "room1",
                        "entry_point_position": [],
                        "entry_point_room": "room1",
                        "execution_time": 0
                    }
                },
            }
        }
        level_content["declarations"] = self.values.declarations
        for room in self.values.rooms:
            level_content["rooms"].append({"room_id": room.values.room_id, "name": room.values.name})
            room.finalize(output_directory, preview)
        level_json = json.dumps(level_content, indent=1)
        with open(level_file, "w") as output_file:
            output_file.write(level_json)
