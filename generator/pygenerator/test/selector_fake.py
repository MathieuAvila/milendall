"""Selector for tests"""

import logging
import cgtypes

from selector import Selector
from room_structure import RoomStructure
from gate_structure import GateStructure
from dressing import Dressing
import concrete_room

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
        self.gate.values.structure_private={}
        self.gate.values.structure_private["private1"] = True

    def check_structure(self):
        """same as main"""
        logging.debug("Check structure gate: %s", self.gate.values.gate_id)

    def check_fit(self):
        """same as main"""
        logging.debug("Check fit gate: %s", self.gate.values.gate_id)
        return True

class GateDressingFake1(Dressing):
    _name = "gate_dressing_1"

    def __init__(self, element=None):
        """ nothing"""
        self._element = element

    def get_instance(self, element=None):
        """ Return instance for a given gate """
        return GateDressingFake1(element)

    def instantiate(self):
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        return True

class RoomDressingFake1(Dressing):
    _name = "room_dressing_1"

    def __init__(self, element=None):
        """ nothing"""
        self._element = element

    def get_instance(self, element=None):
        """ Return instance for a given gate """
        return RoomDressingFake1(element)

    def instantiate(self):
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        return True

    def generate(self, concrete):
        """Instantiate only 1 triangle to pass validity check"""
        parent = concrete.get_node("parent")
        parent.add_dressing_faces(
            [ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ],
            [ [0,1,2] ],
            concrete_room.get_texture_definition("../texture.png"))

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
        self.room.values.structure_private={}
        self.room.values.structure_private["size"] = [10.0,10.0,2.5]
        #for gate in self.room.gates:
        #    logging.info("My gate")

    def check_structure(self):
        """same as main"""
        logging.debug("Check structure room: %s", self.room.values.room_id)

    def check_fit(self):
        """same as main"""
        logging.debug("Check fit room: %s", self.room.values.room_id)
        return True

    def generate(self, concrete):
        """generate 1 structure triangle to be able to check validity"""
        parent = concrete.add_child(None, "parent")
        index0 = parent.add_structure_points([ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ])
        parent.add_structure_faces(
            index0,
            [ [0,1,2], [3,4,5], [6,7,8] ],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], [ 0 ] )

class RoomStructureFake2(RoomStructureFake1):
    _name = "room_structure_2"

    def __init__(self, room=None):
        """ init room """
        super().__init__(room)
        self.room = room

class SelectorFake(Selector):

    def __init__(self):

        self.classes = { 
            "structure": { "room":{}, "gate":{} },
            "dressing": { "room":{}, "gate":{} }
        }

        self.classes["structure"]["room"][RoomStructureFake1().get_name()] = RoomStructureFake1()
        self.classes["structure"]["room"][RoomStructureFake2().get_name()] = RoomStructureFake2()
        self.classes["structure"]["gate"][GateStructureFake1().get_name()] = GateStructureFake1()
        self.classes["structure"]["gate"][GateStructureFake2().get_name()] = GateStructureFake2()

        self.classes["dressing"]["room"][RoomDressingFake1().get_name()] = RoomDressingFake1()
        self.classes["dressing"]["gate"][GateDressingFake1().get_name()] = GateDressingFake1()
