"""
Real room selector based on catalog
"""

import logging

from selector import Selector
from rooms import register as room_register
from gates import register as gate_register
from dressings import register as dressing_register

class SelectorRegular(Selector):

    def __init__(self):
        self.classes["structure"]["room"] = room_register.roomTypes
        self.classes["structure"]["gate"] = gate_register.gateTypes
        # todo: differentiate gates and rooms dressing 
        self.classes["dressing"]["room"] = dressing_register.dressingTypes
        self.classes["dressing"]["gate"] = dressing_register.dressingTypes

        logging.info("Room structure list is: %s", " ".join(self.classes["structure"]["room"]) )
        logging.info("Gate structure list is: %s", " ".join(self.classes["structure"]["gate"]) )
        logging.info("Room dressing list is: %s", " ".join(self.classes["dressing"]["room"]) )
        logging.info("Gate dressing list is: %s", " ".join(self.classes["dressing"]["gate"]) )
