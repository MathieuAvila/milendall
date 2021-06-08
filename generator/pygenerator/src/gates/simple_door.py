"""
structure definition for a simple rectangular room
"""

import logging
from gate_structure import GateStructure
from .register import register_gate_type
import cgtypes.vec3
import concrete_room

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
        self.gate.values.private_parameters["geometry"] =  {
            "portal":[1.5,2.0],
            "margin":[0.5 , 0.5],
            "floor":["x0"],
            "shift": {
                "x_floor_start_ext": 0.0,
                "x_floor_start_int": 0.0,
                "x_floor_end_ext": 0.0,
                "x_floor_end_int": 0.0,

                "x_up_start_ext": 0.0,
                "x_up_start_int": 0.0,
                "x_up_end_ext": 0.0,
                "x_up_end_int": 0.0,

                "y_up_start_ext": 0.0,
                "y_up_start_int": 0.0,
                "y_up_end_ext": 0.0,
                "y_up_end_int": 0.0,
            }
        }

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.private_parameters
        shifts = structure_private["geometry"]["shift"]
        logging.info("generate a door")

        child_object = concrete.add_child(self.gate.get_id(), self.gate.get_id() + "_impl")
        index_wall = child_object.add_structure_points(
                    [
                    cgtypes.vec4(0, 0, 0.5, 0),
                    cgtypes.vec4(0, 1, 0.5, 0),
                    cgtypes.vec4(2, 1, 0.5, 0),
                    cgtypes.vec4(2, 0, 0.5, 0),
                ])
        child_object.add_structure_faces(
            index_wall,
            [ [ 3,2,1,0 ] ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
            [])

register_gate_type(DoorGate())
