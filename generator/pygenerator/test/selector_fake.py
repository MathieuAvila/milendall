"""Selector for tests"""

import logging

from selector import Selector
from room_structure import RoomStructure
from gate_structure import GateStructure

class GateStructureFake1(GateStructure):

    _name = "gate_structure_1"

    def __init__(self, gate=None):
        """ init gate """
        super().__init__(gate)
        self.gate = gate

    def get_instance(self, gate:None):
        """ return a self instance of this gate"""
        return GateStructureFake1(gate)

    def check_fit(self):
        """ Pass the Gate, check it can be applied. """
        return True

    def instantiate(self):
        """ force set values:
        - set values to gate size"""
        self.gate.values.private_parameters={}
        self.gate.values.private_parameters["private1"] = True

class GateStructureFake2(GateStructureFake1):

    _name = "gate_structure_2"

    def __init__(self, gate=None):
        """ init gate """
        super().__init__(gate)
        self.gate = gate

class RoomStructureFake1(RoomStructure):

    _name = "room_structure_1"

    def __init__(self, room=None):
        """ init room """
        super().__init__(room)
        self.room = room

    def get_instance(self, room:None):
        """ return instance"""
        return RoomStructureFake1(room)

    def check_fit(self):
        """ Pass the Room, and list of gates, check it can be applied. """
        logging.info("checking if rectangular fits: always ! rectangular rules the world !")
        return True

    def instantiate(self):
        """ force set values:
        - set values to room size
        - set values for gates"""
        self.room.values.private_parameters={}
        self.room.values.private_parameters["size"] = [10.0,10.0,2.5]
        #for gate in self.room.gates:
        #    logging.info("My gate")

class RoomStructureFake2(RoomStructureFake1):
    _name = "room_structure_2"

    def __init__(self, room=None):
        """ init room """
        super().__init__(room)
        self.room = room

class SelectorFake(Selector):

    def __init__(self):
        self.room_types = {}
        self.room_types[RoomStructureFake1().get_name()] = RoomStructureFake1()
        self.room_types[RoomStructureFake2().get_name()] = RoomStructureFake2()
        self.gate_types = {}
        self.gate_types[GateStructureFake1().get_name()] = GateStructureFake1()
        self.gate_types[GateStructureFake2().get_name()] = GateStructureFake2()

    def get_room_structure_fit(self, _room):
        """ find a list of fits for the room from the real ones """
        list_fit = [self.room_types[name].get_instance(_room)
                    for name in self.room_types]
        return list_fit

    def get_room_structure_from_name(self, name, _room):
        """ return the class from the loaded name"""
        return self.room_types[name].get_instance(_room)

    def get_gate_structure_fit(self, _gate):
        """ find a list of fits for the gate """
        list_fit = [self.gate_types[name].get_instance(_gate)
                    for name in self.gate_types]
        logging.info("Fit list is: %s", str(self.gate_types))
        return list_fit

    def get_gate_structure_from_name(self, name, _gate):
        """ return a gate structure from a name"""
        return self.gate_types[name].get_instance(_gate)
