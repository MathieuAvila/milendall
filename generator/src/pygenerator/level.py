import json
import room
import gate
from munch import DefaultMunch

def decode_level(dct):
    if 'room_id' in dct:
         return room.Room(dct)
    if 'gate_id' in dct:
         return gate.Gate(dct)
    return dct

class JSONEncoder(json.JSONEncoder):

    def default(self, obj):
        return obj.values

class Level:

    def __init__(self, json_file):

        with open(json_file, "r") as read_file:
            obj = json.load(read_file, object_hook=decode_level)
            self.values = DefaultMunch.fromDict(obj)

    def dump_json(self):
        return json.dumps(self, cls=JSONEncoder, indent=1)

    def dump_graph(self, filename):
        """ Dump a file in graphviz format that allows to graphically visualize
            level"""
        with open(filename, "w") as output_file:
            output_file.write("digraph g {\n") 
            output_file.write("node [shape=box];\n")
            [r.dump_graph(output_file) for r in self.values.rooms]
            [g.dump_graph(output_file) for g in self.values.gates]
            output_file.write("}\n")

    def structure_check_coherency(self):
        """ Sanity check that content is viable, at the structure level
            Thing can get insane if user has messed up with content in-betwween

            1. For each room, if room type is selected
            2. gather the gate lists with their format
            3. Check it is allowed by the room type
            4. Request room structure coherency"""
        pass

    def dressing_check_coherency(self):
        """ Sanity check that content is viable, at the structure level
            Thing can get insane if user has messed up with content in-betwween"""
        pass

    def instantiation(self):
        """ 1. For each gate, choose gate format if not already done
            2. Instantiate each room if not already done"""
        pass

    def room_instantiation(self, room_id):
        """ 1. Sort room types that matches constraints: list of gates with format of each. 
            Note: it's up to the room type to check criterias
            2. Associate weights for each
            3. Random selection of one type"""
        pass

    def structure(self):
        """ 1. Sanity check
            2. For each gate, call type to generate sizes
            3. For each room, pass gates information and forced parameters to sekected type
               type will use its private structure information to store types' parameters"""
        pass

    def room_structure(self, room_id):
        pass

    def dressing(self):
        pass

    def room_dressing(self, room_id):
        pass

    def objects(self):
        pass

    def room_objects(self, room_id):
        pass

    def finalize(self):
        pass

    def room_finalize(self, room_id):
        pass
