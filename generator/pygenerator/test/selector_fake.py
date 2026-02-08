"""Selector for tests"""

from __future__ import annotations

import logging
from typing import Any

import cgtypes

from selector import Selector
from brick_structure import BrickStructure
from dressing import Dressing
import concrete_room
from typing_defs import ElementLike, SelectorLike

logger = logging.getLogger("fake")
logger.setLevel(logging.INFO)

class RoomDressingFake1(Dressing):
    _name = "room_dressing_1"

    def __init__(self, element: ElementLike | None = None) -> None:
        """ nothing"""
        self._element = element

    def get_instance(self, element: ElementLike | None = None) -> RoomDressingFake1:
        """ Return instance for a given gate """
        return RoomDressingFake1(element)

    def instantiate(self, selector: SelectorLike) -> bool:
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        return True

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Instantiate only 1 triangle to pass validity check"""
        parent = concrete.get_node("parent")
        parent.add_dressing_faces(
            [ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ],
            [ [0,1,2] ],
            concrete_room.get_texture_definition("../texture.png"))

class BrickStructureFake1(BrickStructure):

    _name = "brick_structure_1"

    def __init__(self, room: ElementLike | None = None) -> None:
        """ init room """
        super().__init__(room)
        self.room = room

    def get_instance(self, room: ElementLike | None = None) -> BrickStructureFake1:
        """ return instance"""
        return BrickStructureFake1(room)

    def instantiate(self, selector: SelectorLike) -> None:
        """ force set values:
        - set values to room size"""
        self.room.values.parameters.structure_private={}
        self.room.values.parameters.structure_private["size"] = [10.0,10.0,2.5]

    def check_structure(self) -> None:
        """same as main"""
        logger.debug("Check structure room: %s", self.room.values.room_id)

    def check_fit(self) -> int:
        """same as main"""
        logger.debug("Check fit room: %s", self.room.values.room_id)
        return 100

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """generate 1 structure triangle to be able to check validity"""
        parent = concrete.add_child(None, "parent")
        if "pads" in self.room.values:
            for pad in self.room.values.pads:
                concrete.add_child("parent", pad.pad_id)
        index0 = parent.add_structure_points([ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ])
        parent.add_structure_faces(
            index0,
            [ [0,1,2], [3,4,5], [6,7,8] ],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], [ 0 ] )

class BrickStructureFake2(BrickStructureFake1):
    _name = "brick_structure_2"

    def __init__(self, room: ElementLike | None = None) -> None:
        """ init room """
        super().__init__(room)
        self.room = room

class SelectorFake(Selector):

    def __init__(self) -> None:

        self.classes = {
            "structure": { "brick":{}, "gate":{} },
            "dressing": { "brick":{}, "gate":{} }
        }

        self.classes["structure"]["brick"][BrickStructureFake1().get_name()] = BrickStructureFake1()
        self.classes["structure"]["brick"][BrickStructureFake2().get_name()] = BrickStructureFake2()

        self.classes["dressing"]["brick"][RoomDressingFake1().get_name()] = RoomDressingFake1()

        self.choice_selector = []
        self.choice_counter = 0

        self.int_selector = []
        self.int_counter = 0

        self.float_selector = []
        self.float_counter = 0.0

    def get_random_choice(self, l: list[Any]) -> Any:
        if len(self.choice_selector) != 0:
            num = self.choice_selector.pop()
            return l[num]
        else:
            n = self.choice_counter
            self.choice_counter += 1
            return l[n % (len(l))]

    def get_random_int(self, min: int, max: int) -> int:
        if len(self.int_selector) != 0:
            return self.int_selector.pop()
        else:
            n = self.int_counter
            self.int_counter += 1
            return min + (n % (max-min))

    def get_random_float(self, min: float, max: float) -> float:
        if len(self.float_selector) != 0:
                return self.float_selector.pop()
        else:
            n = self.float_counter
            self.float_counter += 0.1
            return min + (n % (max-min))
