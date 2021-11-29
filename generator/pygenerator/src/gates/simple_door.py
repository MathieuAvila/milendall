"""
structure definition for a simple rectangular room
"""

import logging
from gate_structure import GateStructure
from .register import register_gate_type
import cgtypes.vec3
import concrete_room
import animation

from jsonmerge import merge

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
        return 100

    def check_structure(self):
        """check everything is as expected.
        """
        logging.info("checking if door is ok: always ! door rules the world !")
        return True

    def instantiate(self, selector):
        """ force set values:
        - set values to gate size"""
        structure_parameters = self._element.values.structure_parameters
        my_default= {}
        my_default["geometry"] =  {
            "portal":[1.5 , 1.5],
            "margin":[ 1.0, 1.0],
            "floor":["x0"]}
        my_default["shift"]= {
                "x_floor_start_ext": -0.3,
                "x_floor_start_int": 0.0,
                "x_floor_end_int": 1.5,
                "x_floor_end_ext": 1.8,

                "x_up_start_ext": -0.3,
                "x_up_start_int": 0.0,
                "x_up_end_int": 1.5,
                "x_up_end_ext": 1.8,

                "y_up_start_ext": 1.6,
                "y_up_start_int": 1.5,
                "y_up_end_ext": 1.6,
                "y_up_end_int": 1.5,

                "w_in" : 0.2,
                "w_out" : -0.2,

                "wd_in" : 0.1,
                "wd_out" : -0.1
            }
        my_default["door"]= {
            "event": "",               # which event it responds to
            "default_open" : True,     # which default state it is when event is False
            "timing" : 1.0,            # delay to open or close
            "auto_open" : False         # add mechanism to automatically open when reaching the door
        }
        self._element.values.structure_private = merge( my_default, structure_parameters)
        p = self._element.values.structure_private
        # in case we're in auto-open mode, let's define the event
        if  p["door"]["auto_open"] == True and p["door"]["event"] == "":
            p["door"]["event"] = self.gate.get_id() + "_event"

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.structure_private
        s = structure_private["shift"]
        logging.info("generate a door")

        block_impl = self.gate.get_id() + "_impl"
        child_object = concrete.add_child(self.gate.get_id(), block_impl)

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
            [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
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
                concrete_room.Node.PORTAL_CONNECT : self._element.values.connect,
                concrete_room.Node.GATE_ID : self._element.values.gate_id
            }
        )
        door = structure_private["door"]
        if ("event" in door) and (door["event"] is not None) and (door["event"] != ""):
            d = structure_private["door"]
            door_impl = self.gate.get_id() + "_door"
            child_door = concrete.add_child(block_impl, door_impl)
            index_door = child_door.add_structure_points(
                    [
                    cgtypes.vec4(s["x_floor_start_int"],   0,                       s["wd_in"], 0),
                    cgtypes.vec4(s["x_up_start_int"],      s["y_up_start_int"],     s["wd_in"], 0),
                    cgtypes.vec4(s["x_up_end_int"],        s["y_up_end_int"],       s["wd_in"], 0),
                    cgtypes.vec4(s["x_floor_end_int"],     0,                       s["wd_in"], 0),

                    cgtypes.vec4(s["x_floor_start_int"],   0,                       s["wd_out"], 0),
                    cgtypes.vec4(s["x_up_start_int"],      s["y_up_start_int"],     s["wd_out"], 0),
                    cgtypes.vec4(s["x_up_end_int"],        s["y_up_end_int"],       s["wd_out"], 0),
                    cgtypes.vec4(s["x_floor_end_int"],     0,                       s["wd_out"], 0),

                    cgtypes.vec4(s["x_floor_start_int"],   0,                       0, 0),
                    cgtypes.vec4(s["x_up_start_int"],      s["y_up_start_int"],     0, 0),
                    cgtypes.vec4(s["x_up_end_int"],        s["y_up_end_int"],       0, 0),
                ])
            child_door.add_structure_faces(
                index_door,
                [
                    [ 3,2,1,0 ],
                    [ 4,5,6,7 ],
                    [ 1,2,6,5 ]
                ],
                concrete_room.Node.CAT_PHYS_VIS,
                [concrete_room.Node.HINT_DOOR, concrete_room.Node.HINT_BUILDING],
                {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )
            anim_open = animation.Animation("open_" + self.gate.get_id(), 0.0, d["timing"], d["event"])
            if not d["default_open"]:
                y_start = -s["y_up_end_int"]+0.01
                y_end = 0.0
            else:
                y_start = 0.0
                y_end = -s["y_up_end_int"]+0.01
            anim_open.append_action(door_impl, anim_open.ACTION_TRANSLATION, [
                { "time":0.0, "value": [0.0, y_start ,0.0] },
                { "time":d["timing"], "value": [0.0, y_end ,0.0] }
            ])

            # check if we're auto-open mode, so define an enter/leave zone
            if d["auto_open"]:
                child_object.add_trigger_box(
                    concrete_room.Node.TRIGGER_ENTER_BOX,
                    concrete_room.Node.TRIGGER_SET_TRUE,
                    cgtypes.vec3(s["x_floor_start_ext"] - 1.0, -0.1,               s["w_out"] - 2.0),
                    cgtypes.vec3(s["x_floor_end_ext"] + 1.0  , s["y_up_end_ext"],  s["w_in"] + 2.0),
                    d["event"])
                child_object.add_trigger_box(
                    concrete_room.Node.TRIGGER_LEAVE_BOX,
                    concrete_room.Node.TRIGGER_SET_FALSE,
                    cgtypes.vec3(s["x_floor_start_ext"] - 1.0, -0.1,               s["w_out"] - 2.0),
                    cgtypes.vec3(s["x_floor_end_ext"] + 1.0  , s["y_up_end_ext"],  s["w_in"] + 2.0),
                    d["event"])

            concrete.add_animation(anim_open)



register_gate_type(DoorGate())
