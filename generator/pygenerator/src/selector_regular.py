"""
Real room selector based on catalog
"""

import logging

from selector import Selector
from rooms import register as room_register
from gates import register as gate_register

class SelectorRegular(Selector):

    def __init__(self):
        pass

    def get_room_structure_fit(self, room):
        """ find a list of fits for the room from the real ones """
        list_fit = [room_register.roomTypes[name].get_instance(room) 
                    for name in room_register.roomTypes
                if room_register.roomTypes[name].get_instance(room).check_fit()]
        logging.info("Fit list is: %s", str(room_register.roomTypes))
        return list_fit

    def get_room_structure_from_name(self, name, room):
        """ return the class from the loaded name"""
        return room_register.roomTypes[name].get_instance(room)

    def get_gate_structure_fit(self, _gate):
        """ find a list of fits for the gate """
        list_fit = [gate_register.gateTypes[name].get_instance(_gate)
                    for name in gate_register.gateTypes
                if gate_register.gateTypes[name].get_instance(_gate).check_fit()]
        logging.info("Fit list is: %s", str(gate_register.gateTypes))
        return list_fit

    def get_gate_structure_from_name(self, name, _gate):
        """ return a gate structure from a name"""
        return gate_register.gateTypes[name].get_instance(_gate)
