"""
Real room selector based on catalog
"""

import logging
import random

from selector import Selector
from bricks import register as room_register
from gates import register as gate_register
from dressings import register as dressing_register

class SelectorRegular(Selector):

    def __init__(self):
        self.classes["structure"]["room"] = room_register.brickTypes
        self.classes["structure"]["gate"] = gate_register.gateTypes
        # todo: differentiate gates and rooms dressing
        self.classes["dressing"]["room"] = dressing_register.dressingTypes
        self.classes["dressing"]["gate"] = dressing_register.dressingTypes

        #logging.info("Room structure list is: %s", " ".join(self.classes["structure"]["room"]) )
        #logging.info("Gate structure list is: %s", " ".join(self.classes["structure"]["gate"]) )
        #logging.info("Room dressing list is: %s", " ".join(self.classes["dressing"]["room"]) )
        #logging.info("Gate dressing list is: %s", " ".join(self.classes["dressing"]["gate"]) )

    def get_random_choice(self, l):
        return random.choice(l)

    def get_random_int(self, min,max):
        """Random number wrapper, to allow UT control over random-itude."""
        return random.randint(min,max)

    def get_random_float(self, min,max):
        """Random number wrapper, to allow UT control over random-itude."""
        return random.random()* (max-min) + min
