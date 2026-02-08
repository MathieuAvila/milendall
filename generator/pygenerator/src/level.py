""" A level definition"""

from __future__ import annotations

from typing import Callable

import logging
import pathlib
import os

import json
import json_helper

import room_spec
from state import LevelState, StateList

from munch import DefaultMunch
import level_instantiation

from typing_defs import LevelValues, RoomSpecLike, SelectorLike

logger = logging.getLogger("level")
logger.setLevel(logging.INFO)

def decode_level(level_directory: str, state: LevelState,
                 selector: SelectorLike) -> Callable[[dict[str, object]], object]:
    """automatically set object type"""
    def _decode_level(dct: dict[str, object]) -> object:

        if 'room_id' in dct:
            return room_spec.RoomSpec(dct, level_directory, state, selector)
        #if 'gate_id' in dct:
        #    return gate.Gate(dct, level_directory, state, selector)
        return dct

    return _decode_level

class Level:

    FILENAME_INSTANTIATED = "level-instantiated.json"
    FILENAME_PERSONALIZED = "level-personalized.json"
    FILENAME_STORY = "level-story.json"

    status_to_filename = {
        LevelState.Story : FILENAME_STORY,
        LevelState.Personalized : FILENAME_PERSONALIZED,
        LevelState.Instantiated : FILENAME_INSTANTIATED,
        LevelState.DressingInstantiated : FILENAME_INSTANTIATED,
        LevelState.DressingPersonalized : FILENAME_INSTANTIATED,
    }

    SCHEMA_STORY = "file_schema_level_story.json"
    SCHEMA_PERSONALIZED = "file_final_level.json"
    SCHEMA_INSTANTIATED = "file_final_level.json"

    status_to_schema = {
        LevelState.Story : SCHEMA_STORY,
        LevelState.Personalized : SCHEMA_PERSONALIZED,
        LevelState.Instantiated : SCHEMA_INSTANTIATED,
        LevelState.DressingInstantiated : SCHEMA_INSTANTIATED,
        LevelState.DressingPersonalized : SCHEMA_INSTANTIATED,
    }

    DUMP_INSTANTIATED = "dump-instantiated.graph"
    DUMP_PERSONALIZED = "dump-personalized.graph"

    status_to_dump_graph = {
        LevelState.Instantiated : DUMP_INSTANTIATED,
        LevelState.Personalized : DUMP_PERSONALIZED,
        LevelState.DressingInstantiated : DUMP_PERSONALIZED,
        LevelState.DressingPersonalized : DUMP_PERSONALIZED,
    }

    status: LevelState
    selector: SelectorLike
    state: StateList
    values: LevelValues
    directory: str

    def __init__(self, _selector: SelectorLike) -> None:
        self.status = LevelState.New
        self.selector = _selector
        self.state = StateList()

    def read_state_list(self, directory: str) -> StateList:
        return StateList(directory + "/state.txt")

    def load(self, directory: str, load_state: LevelState) -> None:
        self.directory = directory
        self.state = StateList(directory + "/state.txt")
        logger.info("Saved states are: %s" % self.state)
        if not self.state.has_state(load_state):
            raise Exception("Level has no saved state %s" % load_state)
        obj = json_helper.load_and_validate_json(
            directory + "/" + self.status_to_filename[load_state],
            self.status_to_schema[load_state],
            decode_hook=decode_level(directory, self.status, self.selector))
        self.status = load_state
        self.values = DefaultMunch.fromDict(obj)  # type: ignore[assignment]
        if self.status > LevelState.Story:
            self.structure_check_coherency()

    def save(self, directory: str) -> None:
        """save to a file"""
        if self.status == LevelState.New:
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

    def dump_graph(self, output_dir: str) -> None:
        """ Dump a file in graphviz format that allows to graphically visualize
            level"""
        output_file_name = output_dir + "/" + self.status_to_dump_graph[self.status]
        with open(output_file_name, "w") as output_file:
            output_file.write("digraph g {\n")
            output_file.write('newrank="true"\n')
            output_file.write("node [shape=box];\n")
            for _r in self.values.rooms:
                _r.dump_graph(output_file)
            output_file.write("}\n")
        value = os.system("dot -Tpng " + output_file_name + " -o " + output_file_name + ".png")
        if value != 0:
            logger.error("Graph generation returned: %i" % value)


    def _check_parameter_presence(self, element: RoomSpecLike, structure_type: str,
                                  parameter_name: str) -> None:
        """Check a parameter is present"""
        logger.debug("Check %s: %s", structure_type, element.values.room_id)
        if element.values[parameter_name] is None:
            raise Exception ("%s has no %s parameter." % (structure_type, parameter_name))

    def get_room(self, _name: str) -> RoomSpecLike:
        """ return room object from its name, otherwise raise exception"""
        room_list = [r for r in self.values.rooms if r.values.room_id == _name ]
        if len(room_list) != 1:
            raise Exception ("More than one room with ID=%s" % _name)
        return room_list[0]

    def structure_check_coherency(self) -> None:
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

    def dressing_check_coherency(self) -> None:
        """ Sanity check that content is viable, at the structure level
            Thing can get insane if user has messed up with content in-between"""
        logger.info("Dressing check coherency")
        for _room in self.values.rooms:
            logger.debug("Check room: %s", _room.values.room_id)
            if _room.values.dressing_class is not None:
                _room.dressing = self.selector.get_dressing_from_name(
                    _room.values.dressing_class,
                    _room)

    def instantiate(self) -> None:
        level_instantiation.instantiate()

    def structure_personalization(self) -> None:
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.rooms:
            _element.structure_personalization()
        self.state.add_state(LevelState.Personalized)
        self.status = LevelState.Personalized

    def objects(self) -> None:
        """ Place objects in room acording to specs"""

    def room_objects(self, room_id: str) -> None:
        """ Place objects in one room acording to specs"""

    def dressing_instantiation(self) -> None:
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.rooms:
            _element.dressing_instantiation()
        self.state.add_state(LevelState.DressingInstantiated)
        self.status = LevelState.DressingInstantiated

    def dressing_personalization(self) -> None:
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        for _element in self.values.rooms:
            _element.dressing_personalization()
        self.state.add_state(LevelState.DressingPersonalized)
        self.status = LevelState.DressingPersonalized

    def run_step(self, to_state: LevelState) -> None:
        if to_state != self.status + 1:
            raise RuntimeError("Invalid state to run from %s" % self.status)
        if to_state == LevelState.Personalized:
            self.structure_personalization()
        elif to_state == LevelState.Instantiated:
            self.instantiate()
        elif to_state == LevelState.DressingInstantiated:
            self.dressing_instantiation()
        elif to_state == LevelState.DressingPersonalized:
            self.dressing_personalization()
        elif to_state == LevelState.Finalize:
            pass
        else:
            raise RuntimeError("Don't know how to reach state: %s" % to_state)

    def preview(self, output_directory: str) -> None:
        if self.status == LevelState.Finalize:
            self.finalize(output_directory, True)
        else:
            self.dump_graph(output_directory)

    def finalize(self, output_directory: str, preview: bool = False) -> None:
        """ Finalize level, generate final output data"""
        self.dressing_check_coherency()
        pathlib.Path(output_directory).mkdir(parents=True, exist_ok=True)
        level_file = output_directory + "/level.json"
        schema = os.path.realpath(os.getcwd() + "/../../schema/file_final_level.json")
        rooms_list: list[dict[str, str]] = []
        level_content: dict[str, object] = {
            "$schema" : schema,
            "version": "1.0",
            "rooms" : rooms_list,
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
            rooms_list.append({"room_id": room.values.room_id})
            room.finalize(output_directory, preview)
        level_json = json.dumps(level_content, indent=1)
        with open(level_file, "w") as output_file:
            output_file.write(level_json)
