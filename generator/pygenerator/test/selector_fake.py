"""Selector for tests"""

import logging
import cgtypes

from selector import Selector
from brick_structure import BrickStructure
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

    def instantiate(self, selector):
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
        return 100

    def generate(self, concrete):
        """generate 1 structure triangle to be able to check validity"""
        parent = concrete.add_child(self.gate.values.gate_id, self.gate.values.gate_id + "_impl")
        index0 = parent.add_structure_points([ cgtypes.vec3(3), cgtypes.vec3(4), cgtypes.vec3(5) ])
        parent.add_structure_faces(
            index0,
            [ [0,1,2], [3,4,5], [6,7,8] ],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], [ 0 ] )

class GateDressingFake1(Dressing):
    _name = "gate_dressing_1"

    def __init__(self, element=None):
        """ nothing"""
        self._element = element

    def get_instance(self, element=None):
        """ Return instance for a given gate """
        return GateDressingFake1(element)

    def instantiate(self, selector):
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        return True

    def generate(self, concrete):
        """Instantiate only 1 triangle to pass validity check"""
        parent = concrete.get_node(self._element.values.gate_id + "_impl")
        parent.add_dressing_faces(
            [ cgtypes.vec3(3), cgtypes.vec3(4), cgtypes.vec3(5) ],
            [ [0,1,2] ],
            concrete_room.get_texture_definition("../texture.png"))

class RoomDressingFake1(Dressing):
    _name = "room_dressing_1"

    def __init__(self, element=None):
        """ nothing"""
        self._element = element

    def get_instance(self, element=None):
        """ Return instance for a given gate """
        return RoomDressingFake1(element)

    def instantiate(self, selector):
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

class BrickStructureFake1(BrickStructure):

    _name = "brick_structure_1"

    def __init__(self, room=None):
        """ init room """
        super().__init__(room)
        self.room = room

    def get_instance(self, room:None):
        """ return instance"""
        return BrickStructureFake1(room)

    def instantiate(self, selector):
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
        return 100

    def generate(self, concrete):
        """generate 1 structure triangle to be able to check validity"""
        parent = concrete.add_child(None, "parent")
        index0 = parent.add_structure_points([ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ])
        parent.add_structure_faces(
            index0,
            [ [0,1,2], [3,4,5], [6,7,8] ],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], [ 0 ] )

class BrickStructureFake2(BrickStructureFake1):
    _name = "brick_structure_2"

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

        self.classes["structure"]["room"][BrickStructureFake1().get_name()] = BrickStructureFake1()
        self.classes["structure"]["room"][BrickStructureFake2().get_name()] = BrickStructureFake2()
        self.classes["structure"]["gate"][GateStructureFake1().get_name()] = GateStructureFake1()
        self.classes["structure"]["gate"][GateStructureFake2().get_name()] = GateStructureFake2()

        self.classes["dressing"]["room"][RoomDressingFake1().get_name()] = RoomDressingFake1()
        self.classes["dressing"]["gate"][GateDressingFake1().get_name()] = GateDressingFake1()

        self.choice_selector = []
        self.choice_counter = 0

        self.int_selector = []
        self.int_counter = 0

        self.float_selector = []
        self.float_counter = 0.0

    def get_random_choice(self, l):
        if len(self.choice_selector) != 0:
            num = self.choice_selector.pop()
            return l[num]
        else:
            n = self.choice_counter
            self.choice_counter += 1
            return l[n % (len(l))]

    def get_random_int(self, min,max):
        if len(self.int_selector) != 0:
            return self.int_selector.pop()
        else:
            n = self.int_counter
            self.int_counter += 1
            return min + (n % (max-min))

    def get_random_float(self, min,max):
        if len(self.float_selector) != 0:
                return self.float_selector.pop()
        else:
            n = self.float_counter
            self.float_counter += 0.1
            return min + (n % (max-min))
