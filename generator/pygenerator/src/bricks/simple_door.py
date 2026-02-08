"""
structure definition for a simple rectangular room
"""

from __future__ import annotations

import logging
import math

from brick_structure import BrickStructure
from .register import register_brick_type
import cgtypes.vec3
import cgtypes.mat4
import concrete_room
import animation

from jsonmerge import merge
from typing_defs import ElementWithValues, SelectorLike

logger = logging.getLogger("simple_door")
logger.setLevel(logging.INFO)

class SimpleDoorBrick(BrickStructure):

    _name = "simple_door"

    _element: ElementWithValues | None
    brick: ElementWithValues | None

    def __init__(self, brick: ElementWithValues | None = None) -> None:
        """ init simple door"""
        super().__init__(brick)
        self.brick = brick
        self._element = brick

    def get_instance(self, brick: ElementWithValues) -> SimpleDoorBrick:
        """ return a self instance of this brick"""
        return SimpleDoorBrick(brick)

    def check_fit(self) -> int:
        """ Pass the brick, check it can be applied. """
        logger.debug("checking if door fits: always ! door rules the world !")
        return 100

    def check_structure(self) -> bool:
        """check everything is as expected.
        """
        if self._element is None:
            raise RuntimeError("SimpleDoorBrick requires an element to check structure")
        v = self._element.values
        if "portals" not in v:
            raise Exception("simple_door '%s' needs a portals" % self._element.get_id())
        p = v.portals[0]
        if "gate_id" not in p:
            raise Exception("simple_door '%s' needs a gate_id" % self._element.get_id())
        if "connect" not in p:
            raise Exception("simple_door '%s' needs a connect field" % self._element.get_id())

        logger.debug("checking if door is ok: always ! door rules the world !")
        return True

    def instantiate(self, selector: SelectorLike) -> None:
        """ force set values:
        - set values to brick size"""
        if self._element is None:
            raise RuntimeError("SimpleDoorBrick requires an element to instantiate")
        self.check_structure()
        structure_parameters = self._element.values.parameters.structure_parameters
        my_default= {}
        my_default["geometry"] =  {
            "portal":[1.5 , 1.5],
            "margin":[ 1.0, 1.0],
            "floor":["x0"]}
        my_default["shift"]= {
                "x_floor_start_ext": -0.2,
                "x_floor_start_int": 0.1,
                "x_floor_end_int": 1.6,
                "x_floor_end_ext": 1.9,

                "x_up_start_ext": -0.2,
                "x_up_start_int": 0.1,
                "x_up_end_int": 1.6,
                "x_up_end_ext": 1.9,

                "y_up_start_ext": 1.8,
                "y_up_start_int": 1.55,
                "y_up_end_ext": 1.8,
                "y_up_end_int": 1.55,

                "w_in" : 0.3,
                "w_out" : -0.3,

                "wd_in" : 0.1,
                "wd_out" : -0.1
            }
        my_default["door"]= {
            "event": "",               # which event it responds to
            "default_open" : True,     # which default state it is when event is False
            "timing" : 1.0,            # delay to open or close
            "auto_open" : False         # add mechanism to automatically open when reaching the door
        }
        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)
        p = self._element.values.parameters.structure_private
        # in case we're in auto-open mode, let's define the event
        if  p["door"]["auto_open"] == True and p["door"]["event"] == "":
            p["door"]["event"] = self.brick.get_id() + "_event"

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""
        if self._element is None or self.brick is None:
            raise RuntimeError("SimpleDoorBrick requires an element to generate")
        structure_private = self._element.values.parameters.structure_private
        s = structure_private["shift"]
        portal = self._element.values.portals[0]

        mat = None
        if portal["connect"] == "B":
            mat = cgtypes.mat4.translation([s["x_floor_end_int"], 0.0, 0.0]) * cgtypes.mat4.rotation(math.pi, cgtypes.vec3(0.0, 1.0, 0.0))


        child_object = concrete.add_child(None, self.brick.get_id(), mat)

        index_wall = child_object.add_structure_points(
                    [
                    cgtypes.vec3(s["x_floor_start_ext"],   0,                       s["w_in"]),
                    cgtypes.vec3(s["x_floor_start_int"],   0,                       s["w_in"]),
                    cgtypes.vec3(s["x_up_start_int"],      s["y_up_start_int"],     s["w_in"]),
                    cgtypes.vec3(s["x_up_end_int"],        s["y_up_end_int"],       s["w_in"]),
                    cgtypes.vec3(s["x_floor_end_int"],     0,                       s["w_in"]),
                    cgtypes.vec3(s["x_floor_end_ext"],     0,                       s["w_in"]),
                    cgtypes.vec3(s["x_up_end_ext"],        s["y_up_end_ext"],       s["w_in"]),
                    cgtypes.vec3(s["x_up_start_ext"],      s["y_up_start_ext"],     s["w_in"]),

                    cgtypes.vec3(s["x_floor_start_ext"],   0,                       s["w_out"]),
                    cgtypes.vec3(s["x_floor_start_int"],   0,                       s["w_out"]),
                    cgtypes.vec3(s["x_up_start_int"],      s["y_up_start_int"],     s["w_out"]),
                    cgtypes.vec3(s["x_up_end_int"],        s["y_up_end_int"],       s["w_out"]),
                    cgtypes.vec3(s["x_floor_end_int"],     0,                       s["w_out"]),
                    cgtypes.vec3(s["x_floor_end_ext"],     0,                       s["w_out"]),
                    cgtypes.vec3(s["x_up_end_ext"],        s["y_up_end_ext"],       s["w_out"]),
                    cgtypes.vec3(s["x_up_start_ext"],      s["y_up_start_ext"],     s["w_out"]),

                    cgtypes.vec3(s["x_floor_start_int"],   0,                       ),
                    cgtypes.vec3(s["x_up_start_int"],      s["y_up_start_int"],     ),
                    cgtypes.vec3(s["x_up_end_int"],        s["y_up_end_int"],       ),
                    cgtypes.vec3(s["x_floor_end_ext"],     0,                       ),
                ])
        child_object.add_structure_faces(
            index_wall,
            [
                [ 0,1,2,7 ],
                [ 2,3,6,7 ],
                [ 4,5,6,3 ],

                [ 8+2,8+7,8+0,8+1 ][::-1],
                [ 8+6,8+7,8+2,8+3 ][::-1],
                [ 8+6,8+3,8+4,8+5 ][::-1],

                [ 2,10, 9, 1][::-1],
                [ 2,3,11,10 ][::-1],
                [ 4,12,11,3 ][::-1],

                [ 5,6,14,13 ][::-1],
                [ 6,7,15,14 ][::-1],
                [ 7,0,8,15 ][::-1],

                [ 0,1,9,8 ][::-1],
                [ 4,5,5+8,4+8 ][::-1],

            ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
            {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        if portal["connect"] == "B":
            list_portal = [ 16,17,18,19 ]
        else:
            list_portal = [ 19,18,17,16 ]
        child_object.add_structure_faces(
            index_wall,
            [
                list_portal,
            ],
            concrete_room.Node.CAT_PHYS,
            [],
            {
                concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_PORTAL,
                concrete_room.Node.PORTAL_CONNECT : portal["connect"],
                concrete_room.Node.GATE_ID : portal["gate_id"]
            }
        )
        door = structure_private["door"]
        if ("event" in door) and (door["event"] is not None) and (door["event"] != ""):
            d = structure_private["door"]
            door_impl = self.brick.get_id() + "_door"
            child_door = concrete.add_child(None, door_impl)
            index_door = child_door.add_structure_points(
                    [
                    cgtypes.vec3(s["x_floor_start_int"],   0,                       s["wd_in"]),
                    cgtypes.vec3(s["x_up_start_int"],      s["y_up_start_int"],     s["wd_in"]),
                    cgtypes.vec3(s["x_up_end_int"],        s["y_up_end_int"],       s["wd_in"]),
                    cgtypes.vec3(s["x_floor_end_int"],     0,                       s["wd_in"]),

                    cgtypes.vec3(s["x_floor_start_int"],   0,                       s["wd_out"]),
                    cgtypes.vec3(s["x_up_start_int"],      s["y_up_start_int"],     s["wd_out"]),
                    cgtypes.vec3(s["x_up_end_int"],        s["y_up_end_int"],       s["wd_out"]),
                    cgtypes.vec3(s["x_floor_end_int"],     0,                       s["wd_out"]),

                    cgtypes.vec3(s["x_floor_start_int"],   0,                       0),
                    cgtypes.vec3(s["x_up_start_int"],      s["y_up_start_int"],     0),
                    cgtypes.vec3(s["x_up_end_int"],        s["y_up_end_int"],       0),
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
            anim_open = animation.Animation("open_" + self.brick.get_id(), 0.0, d["timing"], d["event"])
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



register_brick_type(SimpleDoorBrick())
