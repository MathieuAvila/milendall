"""
Real room selector based on catalog
"""

import logging

from selector import Selector
from rooms import register as room_register
from gates import register as gate_register

class SelectorRegular(Selector):

    def __init__(self):
        self.classes={}
        self.classes["room"] = room_register.roomTypes
        self.classes["gate"] = gate_register.gateTypes

    def get_structure_fit(self, element):
        """ find a list of fits for the structure from the real ones """
        element_list = self.classes[element.get_class()]
        list_fit = [element_list[name].get_instance(element)
                    for name in element_list
                if element_list[name].get_instance(element).check_fit()]
        logging.info("Fit list is: %s", str(list_fit))
        return list_fit

    def get_structure_from_name(self, name, element):
        """ return the class from the loaded name"""
        element_class = element.get_class()
        if name not in self.classes[element_class]:
            raise Exception("Element of class: %s has no class of type : %s. Classes are: %s" %
                (element_class, name, str(self.classes[element_class].keys())))
        return self.classes[element_class][name].get_instance(element)
