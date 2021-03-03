"""
structure definition for a simple rectangular room
"""

import logging
from gate_structure import GateStructure
from .register import register_gate_type

class DoorGate(GateStructure):

    _name = "simple_door"

    def __init__(self, gate=None):
        """ init gate """
        super().__init__(gate)
        self.gate = gate

    def get_instance(self, gate:None):
        """ return a self instance of this gate"""
        return DoorGate(gate)

    def check_fit(self):
        """ Pass the Gate, check it can be applied. """
        logging.info("checking if door fits: always ! door rules the world !")
        return True

    def check_structure(self):
        """check everything is as expected.
        """
        logging.info("checking if door is ok: always ! door rules the world !")
        return True

    def instantiate(self):
        """ force set values:
        - set values to gate size"""
        self.gate.values.private_parameters={}
        self.gate.values.private_parameters["size"] = [2.0, 2.0]



register_gate_type(DoorGate())
