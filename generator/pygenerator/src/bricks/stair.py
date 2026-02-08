"""
structure definition for a simple stair brick
"""

from __future__ import annotations

import logging
import math

from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

import milendall_math
import gltf_helper

from .register import register_brick_type

from jsonmerge import merge
from typing_defs import ElementWithValues, SelectorLike

logger = logging.getLogger("stair")
logger.setLevel(logging.INFO)

class BrickStair(BrickStructure):

    _name = "stair"

    _element: ElementWithValues | None

    def __init__(self, _element: ElementWithValues | None = None) -> None:
        """ init brick """
        self._element = _element

    def get_instance(self, brick: ElementWithValues) -> BrickStair:
        """Return an instante"""
        return BrickStair(brick)

    def check_structure(self) -> bool:
        """check everything is as expected.
        """
        logger.debug("checking if stair is ok")
        return True

    def instantiate(self, selector: SelectorLike) -> None:
        """ force set values:
        - set values to brick size"""
        if self._element is None:
            raise RuntimeError("BrickStair requires an element to instantiate")
        structure_parameters = self._element.values.parameters.structure_parameters

        my_default= {
            "x_length"  : 5.0,
            "y_height"  : 3.0,
            "nr_steps"  : 15,
            "y_base_start"   : -0.3,
            "y_base_end"     : 2.7,
            "x_pre"     : 1.0,
            "x_post"    : 1.0,
            "z0"        : 0.0,
            "z1"        : 1.5
        }
        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""
        if self._element is None:
            raise RuntimeError("BrickStair requires an element to generate")
        structure_private = self._element.values.parameters.structure_private

        # create main object
        parent = concrete.add_child(None, "parent")

        wall_kind = [
            concrete_room.Node.HINT_WALL,
            ]
        pads = self._element.values.pads

        z0 = structure_private["z0"]
        z1 = structure_private["z1"]
        x_end = structure_private["x_length"]
        y_end = structure_private["y_height"]
        nr = structure_private["nr_steps"]
        y_base_start = structure_private["y_base_start"]
        y_base_end = structure_private["y_base_end"]

        for stair in range(0, nr):
            xs = (x_end) * stair / nr
            ys = (y_end) * stair / nr
            xe = (x_end) * (stair + 1) / nr
            ye = (y_end) * (stair + 1) / nr
            ybs = y_base_start + (y_base_end - y_base_start) * (stair) / nr
            ybe = y_base_start + (y_base_end - y_base_start) * (stair + 1) / nr
            index = parent.add_structure_points([
                    cgtypes.vec3(xs, ys, z0),
                    cgtypes.vec3(xs, ys, z1),

                    cgtypes.vec3(xs, ye, z0),
                    cgtypes.vec3(xs, ye, z1),

                    cgtypes.vec3(xe, ye, z0),
                    cgtypes.vec3(xe, ye, z1),

                    cgtypes.vec3(xs, ybs, z0),
                    cgtypes.vec3(xs, ybs, z1),

                    cgtypes.vec3(xe, ybe, z0),
                    cgtypes.vec3(xe, ybe, z1),
            ])

            parent.add_structure_faces(
                index,
                [
                    [0,1,3,2], # vertical face
                    [2,3,5,4], # horiz face
                    [2,4,8,6], # border z0
                    [7,9,5,3], # border z1
                ],
                concrete_room.Node.CAT_PHYS_VIS,
                [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )


        # add under border
        index = parent.add_structure_points([
                    cgtypes.vec3(0, y_base_start, z0),
                    cgtypes.vec3(0, y_base_start, z1),

                    cgtypes.vec3(x_end, y_base_end, z0),
                    cgtypes.vec3(x_end, y_base_end, z1)
            ])
        parent.add_structure_faces(
                index,
                [
                    [2,3,1,0], # up

                ],
                concrete_room.Node.CAT_PHYS_VIS,
                [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        # extend if pre
        pre = structure_private["x_pre"]

        if (pre != 0.0):
            index = parent.add_structure_points([
                    cgtypes.vec3(0, 0, z0),
                    cgtypes.vec3(0, 0, z1),

                    cgtypes.vec3(-pre, 0, z0),
                    cgtypes.vec3(-pre, 0, z1),

                    cgtypes.vec3(-pre, y_base_start, z0),
                    cgtypes.vec3(-pre, y_base_start, z1),

                    cgtypes.vec3(0, y_base_start, z0),
                    cgtypes.vec3(0, y_base_start, z1),
            ])
            parent.add_structure_faces(
                index,
                [
                    [2,3,1,0], # up
                    [6,7,5,4], # down
                    [4,5,3,2], # border x=pre
                    [6,4,2,0], # border z=z0
                    [1,3,5,7], # border z=z1
                ],
                concrete_room.Node.CAT_PHYS_VIS,
                [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        # extend if post
        post = structure_private["x_post"]

        if (post != 0.0):
            index = parent.add_structure_points([
                    cgtypes.vec3(x_end + post, y_end, z0),
                    cgtypes.vec3(x_end + post, y_end, z1),

                    cgtypes.vec3(x_end, y_end, z0),
                    cgtypes.vec3(x_end, y_end, z1),

                    cgtypes.vec3(x_end, y_base_end, z0),
                    cgtypes.vec3(x_end, y_base_end, z1),

                    cgtypes.vec3(x_end + post, y_base_end, z0),
                    cgtypes.vec3(x_end + post, y_base_end, z1),
            ])
            parent.add_structure_faces(
                index,
                [
                    [2,3,1,0], # up
                    [6,7,5,4], # down
                    [0,1,7,6], # border x=post
                    [6,4,2,0], # border z=z0
                    [1,3,5,7], # border z=z1
                ],
                concrete_room.Node.CAT_PHYS_VIS,
                [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )


register_brick_type(BrickStair())
