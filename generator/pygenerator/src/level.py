""" A level definition"""

import logging
import random

import json

import room
import gate
from munch import DefaultMunch

def decode_level(dct):
    """automatically set object  type"""
    if 'room_id' in dct:
        return room.Room(dct)
    if 'gate_id' in dct:
        return gate.Gate(dct)
    return dct

class JSONEncoder(json.JSONEncoder):

    def default(self, o):
        return o.values

class Level:

    def __init__(self, json_file):

        with open(json_file, "r") as read_file:
            obj = json.load(read_file, object_hook=decode_level)
            self.values = DefaultMunch.fromDict(obj)
            self.structure_check_coherency()

    def dump_json(self):
        """ dump internal state for later use"""
        return json.dumps(self, cls=JSONEncoder, indent=1)

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

    def structure_check_coherency(self):
        """ Sanity check that content is viable, at the structure level
            Thing can get insane if user has messed up with content in-between

            1. For each room, if room type is selected
            2. gather the gate lists with their format
            3. Check it is allowed by the room type
            4. Request room structure coherency"""
        logging.info("Check coherency")
        for _gate in self.values.gates:
            if _gate.values.gate_id is None:
                raise Exception ("Gate has no gate_id parameter.")
            logging.info("Check gate: %s", _gate.values.gate_id)

    def dressing_check_coherency(self):
        """ Sanity check that content is viable, at the structure level
            Thing can get insane if user has messed up with content in-between"""

    def _room_instantiation(self, room_selector, _room):
        """ 1. Sort room types that matches constraints: list of gates with format of each.
            Note: it's up to the room type to check criterias
            2. Associate weights for each
            3. Random selection of one type"""
        if _room.values.structure_class is None:
            logging.info("Need to select class for room: %s", _room.values.room_id)
            fit_list = room_selector.get_room_fit(_room)
            logging.info("Fit list is: %s", str(fit_list))
            if fit_list == []:
                logging.error("Fit list is void, cannot choose any structure."
                              "This is unrecoverable.")
                raise Exception ("Void fit list for room: " + _room.values.room_id)
            # just random selection for the moment
            choice = random.choice(fit_list)
            _room.values.structure_class = choice.get_name()
        else:
            logging.info("No need to select class for room: %s", _room.values.room_id)
            choice = room_selector.get_from_name(room.values.structure_class, room)
            if choice is None:
                logging.error("Unknown class name: %s", _room.values.structure_class)
                raise Exception ("Void fit list for room: " + _room.values.room_id)
        if _room.values.private_parameters is None:
            room.values.private_parameters = {}
            choice.instantiate()


    def instantiation(self, room_selector):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        assert room_selector is not None
        for _room in self.values.rooms:
            self._room_instantiation(room_selector, _room)

    def room_instantiation(self, room_selector, room_id):
        """ wrapper to _room_instantiation, with room name resolved"""

    def structure(self):
        """ 1. Sanity check
            2. For each gate, call type to generate sizes
            3. For each room, pass gates information and forced parameters to sekected type
               type will use its private structure information to store types' parameters"""

    def room_structure(self, room_id):
        """ Perform structure only on one room"""

    def dressing(self):
        """ Perform final dressing on every room and gate"""

    def room_dressing(self, room_id):
        """ Perform final dressing one roomm"""

    def objects(self):
        """ Place objects in room acording to specs"""

    def room_objects(self, room_id):
        """ Place objects in one room acording to specs"""

    def finalize(self):
        """ Finalize all level"""

    def room_finalize(self, room_id):
        """ Finalize only one room"""
