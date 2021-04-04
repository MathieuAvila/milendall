"""
structure definition for a simple rectangular room
"""

import logging

from dressing import Dressing
from .register import register_dressing_type

class DressingBasic(Dressing):

    _name = "basic"

    def __init__(self, element=None):
        """ init dressing """
        #super().__init__(element)
        self.element = element

    def get_instance(self, element:None):
        """ return a self instance of this dressing, operating on any object"""
        return DressingBasic(element)

    def instantiate(self):
        """ force set values:
        - set default values to dressing"""
        self.element.values.private_dressing={}
        #self.element.values.private_dressing["size"] = [2.0, 2.0]

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        for obj in concrete.get_objects():
            logging.info("treating object: %s", obj.name)




register_dressing_type(DressingBasic())
