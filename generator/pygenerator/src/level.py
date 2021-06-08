""" A level definition"""

import logging
import pathlib
import os

import json
import jsonschema

import room
import gate
import concrete_room

from munch import DefaultMunch

def decode_level(dct):
    """automatically set object type"""
    if 'room_id' in dct:
        return room.Room(dct)
    if 'gate_id' in dct:
        return gate.Gate(dct)
    return dct

class JSONEncoder(json.JSONEncoder):

    def default(self, o):
        return o.values

class Level:

    def __init__(self, json_file, _selector):

        self.selector = _selector

        with open("../../schema/generator.level.schema.json", "r") as read_schema_file:
            schema = json.load(read_schema_file)

        with open(json_file, "r") as read_file:
            obj = json.load(read_file)
            jsonschema.validate(instance=obj, schema=schema)

        with open(json_file, "r") as read_file:
            obj = json.load(read_file, object_hook=decode_level)
            self.values = DefaultMunch.fromDict(obj)
            self.structure_check_coherency()

    def dump_json(self):
        """dump internal state for later use"""
        return json.dumps(self, cls=JSONEncoder, indent=1)

    def save(self, filename):
        """save to a file"""
        _j = self.dump_json()
        with open(filename, "w") as output_file:
            output_file.write(_j)

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
        logging.info("Check %s: %s", structure_type, element.values.gate_id)
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
        logging.debug("Structure check coherency")
        for _gate in self.values.gates:
            logging.debug("Check gate: %s", _gate.values.gate_id)

            # append gate to both rooms
            room_from = self.get_room(_gate.values.connect[0])
            room_to = self.get_room(_gate.values.connect[1])
            room_from.gates.append(_gate)
            room_to.gates.append(_gate)

            if _gate.values.structure_class is not None:
                _gate.structure = self.selector.get_structure_from_name(
                    _gate.values.structure_class,
                    _gate)
                _gate.structure.check_structure()

        for _room in self.values.rooms:
            logging.debug("Check room: %s", _room.values.room_id)
            if _room.values.structure_class is not None:
                _room.structure = self.selector.get_structure_from_name(
                    _room.values.structure_class,
                    _room)
                _room.structure.check_structure()

    def dressing_check_coherency(self):
        """ Sanity check that content is viable, at the structure level
            Thing can get insane if user has messed up with content in-between"""
        logging.info("Dressing check coherency")
        for _gate in self.values.gates:
            logging.debug("Check gate: %s", _gate.values.gate_id)

            if _gate.values.dressing_class is not None:
                _gate.dressing = self.selector.get_dressing_from_name(
                    _gate.values.dressing_class,
                    _gate)

        for _room in self.values.rooms:
            logging.debug("Check room: %s", _room.values.room_id)
            if _room.values.dressing_class is not None:
                _room.dressing = self.selector.get_dressing_from_name(
                    _room.values.dressing_class,
                    _room)

    def _element_instantiation(self, _element):
        _element.instantiation(self.selector)

    def instantiation(self):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        assert self.selector is not None

        for _element in self.values.gates + self.values.rooms:
            self._element_instantiation(_element)

    def element_instantiation(self, _id):
        """ wrapper to _element_instantiation, with ID name resolved"""
        element = None
        self._element_instantiation(element)

    def objects(self):
        """ Place objects in room acording to specs"""

    def room_objects(self, room_id):
        """ Place objects in one room acording to specs"""

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
            level_content["rooms"].append({"room_id": room.get_id(), "name": room.values.name})
            room.finalize(output_directory, preview)
        level_json = json.dumps(level_content, indent=1)
        with open(level_file, "w") as output_file:
            output_file.write(level_json)
