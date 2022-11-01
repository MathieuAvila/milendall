"""
Real room selector based on catalog
"""

import logging
import random

from selector import Selector
from bricks import register as room_register
from dressings import register as dressing_register

class SelectorRegular(Selector):

    def __init__(self):
        self.classes["structure"]["brick"] = room_register.brickTypes
        self.classes["dressing"]["brick"] = dressing_register.dressingTypes

        #logger.info("Room structure list is: %s", " ".join(self.classes["structure"]["brick"]) )
        #logger.info("Room dressing list is: %s", " ".join(self.classes["dressing"]["brick"]) )

    def get_random_choice(self, l):
        return random.choice(l)

    def get_random_int(self, min,max):
        """Random number wrapper, to allow UT control over random-itude."""
        return random.randint(min,max)

    def get_random_float(self, min,max):
        """Random number wrapper, to allow UT control over random-itude."""
        return random.random()* (max-min) + min
