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

    def instantiate(self):
        """ force set values:
        - set values to gate size"""
        self.gate.values.private_parameters={}
        self.gate.values.private_parameters["private1"] = True

    def check_structure(self):
        """same as main"""
        logging.debug("Check structure gate: %s", self.gate.values.gate_id)

    def check_fit(self):
        """same as main"""
        logging.debug("Check fit gate: %s", self.gate.values.gate_id)
        return True

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

    def instantiate(self):
        """ force set values:
        - set values to room size
        - set values for gates"""
        self.room.values.private_parameters={}
        self.room.values.private_parameters["size"] = [10.0,10.0,2.5]
        #for gate in self.room.gates:
        #    logging.info("My gate")

    def check_structure(self):
        """same as main"""
        logging.debug("Check structure room: %s", self.room.values.room_id)

    def check_fit(self):
        """same as main"""
        logging.debug("Check fit room: %s", self.room.values.room_id)
        return True

class RoomStructureFake2(RoomStructureFake1):
    _name = "room_structure_2"

    def __init__(self, room=None):
        """ init room """
        super().__init__(room)
        self.room = room

class SelectorFake(Selector):

    def __init__(self):
        self.classes={}
        self.classes["room"] = {}
        self.classes["gate"] = {}
        self.classes["room"][RoomStructureFake1().get_name()] = RoomStructureFake1()
        self.classes["room"][RoomStructureFake2().get_name()] = RoomStructureFake2()
        self.classes["gate"][GateStructureFake1().get_name()] = GateStructureFake1()
        self.classes["gate"][GateStructureFake2().get_name()] = GateStructureFake2()
