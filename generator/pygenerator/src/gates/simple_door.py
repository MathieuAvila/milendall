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

    def instantiate(self, selector):
        """ force set values:
        - set values to gate size"""
        self.gate.values.structure_private={}
        self.gate.values.structure_private["geometry"] =  {
            "portal":[2.5 ,2.5],
            "margin":[ 1.0, 1.0],
            "floor":["x0"],
            "shift": {
                "x_floor_start_ext": -0.5,
                "x_floor_start_int": 0.1,
                "x_floor_end_int": 2.4,
                "x_floor_end_ext": 3.0,

                "x_up_start_ext": -0.3,
                "x_up_start_int": 0.2,
                "x_up_end_int": 2.3,
                "x_up_end_ext": 2.8,

                "y_up_start_ext": 2.7,
                "y_up_start_int": 2.5,
                "y_up_end_ext": 2.7,
                "y_up_end_int": 2.5,

                "w_in" : 0.2,
                "w_out" : -0.2
            }
        }

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.structure_private
        s = structure_private["geometry"]["shift"]
        logging.info("generate a door")

        child_object = concrete.add_child(self.gate.get_id(), self.gate.get_id() + "_impl")

        index_wall = child_object.add_structure_points(
                    [
                    cgtypes.vec4(s["x_floor_start_ext"],   0,                       s["w_in"], 0),
                    cgtypes.vec4(s["x_floor_start_int"],   0,                       s["w_in"], 0),
                    cgtypes.vec4(s["x_up_start_int"],      s["y_up_start_int"],     s["w_in"], 0),
                    cgtypes.vec4(s["x_up_end_int"],        s["y_up_end_int"],       s["w_in"], 0),
                    cgtypes.vec4(s["x_floor_end_int"],     0,                       s["w_in"], 0),
                    cgtypes.vec4(s["x_floor_end_ext"],     0,                       s["w_in"], 0),
                    cgtypes.vec4(s["x_up_end_ext"],        s["y_up_end_ext"],       s["w_in"], 0),
                    cgtypes.vec4(s["x_up_start_ext"],      s["y_up_start_ext"],     s["w_in"], 0),

                    cgtypes.vec4(s["x_floor_start_ext"],   0,                       s["w_out"], 0),
                    cgtypes.vec4(s["x_floor_start_int"],   0,                       s["w_out"], 0),
                    cgtypes.vec4(s["x_up_start_int"],      s["y_up_start_int"],     s["w_out"], 0),
                    cgtypes.vec4(s["x_up_end_int"],        s["y_up_end_int"],       s["w_out"], 0),
                    cgtypes.vec4(s["x_floor_end_int"],     0,                       s["w_out"], 0),
                    cgtypes.vec4(s["x_floor_end_ext"],     0,                       s["w_out"], 0),
                    cgtypes.vec4(s["x_up_end_ext"],        s["y_up_end_ext"],       s["w_out"], 0),
                    cgtypes.vec4(s["x_up_start_ext"],      s["y_up_start_ext"],     s["w_out"], 0),

                    cgtypes.vec4(s["x_floor_start_int"],   0,                       0, 0),
                    cgtypes.vec4(s["x_up_start_int"],      s["y_up_start_int"],     0, 0),
                    cgtypes.vec4(s["x_up_end_int"],        s["y_up_end_int"],       0, 0),
                    cgtypes.vec4(s["x_floor_end_ext"],     0,                       0, 0),
                ])
        child_object.add_structure_faces(
            index_wall,
            [
                [ 0,1,2,7 ],
                [ 2,3,6,7 ],
                [ 3,4,5,6 ],

                [ 8+0,8+1,8+2,8+7 ][::-1],
                [ 8+2,8+3,8+6,8+7 ][::-1],
                [ 8+3,8+4,8+5,8+6 ][::-1],

                [ 1,2,10, 9][::-1],
                [ 2,3,11,10 ][::-1],
                [ 3,4,12,11 ][::-1],

                [ 5,6,14,13 ][::-1],
                [ 6,7,15,14 ][::-1],
                [ 7,0,8,15 ][::-1],
            ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_GROUND, concrete_room.Node.HINT_BUILDING],
            {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        child_object.add_structure_faces(
            index_wall,
            [
                [ 16,17,18,19 ],
            ],
            concrete_room.Node.CAT_PHYS,
            [],
            {
                concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_PORTAL,
                concrete_room.Node.PORTAL_CONNECT : self._element.values.connect
            }
        )


register_gate_type(DoorGate())
