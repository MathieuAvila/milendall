"""
Real room selector based on catalog
"""

from __future__ import annotations

import logging
import random
from typing import Sequence

from selector import Selector
from bricks import register as room_register
from dressings import register as dressing_register
from typing_defs import DressingLike, StructureLike

class SelectorRegular(Selector):

    def __init__(self) -> None:
        self.classes["structure"]["brick"] = room_register.brickTypes
        self.classes["dressing"]["brick"] = dressing_register.dressingTypes

        #logger.debug("Room structure list is: %s", " ".join(self.classes["structure"]["brick"]) )
        #logger.debug("Room dressing list is: %s", " ".join(self.classes["dressing"]["brick"]) )

    def get_random_choice(self, l: Sequence[DressingLike]) -> DressingLike:
        return random.choice(l)

    def get_random_int(self, min: int, max: int) -> int:
        """Random number wrapper, to allow UT control over random-itude."""
        return random.randint(min,max)

    def get_random_float(self, min: float, max: float) -> float:
        """Random number wrapper, to allow UT control over random-itude."""
        return random.random()* (max-min) + min
