"""
structure definition for a parametric stair brick
"""

from __future__ import annotations

import functools
import logging
import math

from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4


from .register import register_brick_type

from merge_utils import merge
from typing_defs import ElementWithValues, SelectorLike

logger = logging.getLogger("stair_sampler")
logger.setLevel(logging.INFO)

class BrickStairSampler(BrickStructure):

    _name = "stair_sampler"

    _element: ElementWithValues | None

    def __init__(self, _element: ElementWithValues | None = None) -> None:
        """ init brick """
        self._element = _element

    def get_instance(self, brick: ElementWithValues) -> BrickStairSampler:
        """Return an instance"""
        return BrickStairSampler(brick)

    def check_structure(self) -> bool:
        """check everything is as expected.
        """
        logger.debug("checking if stair is ok")
        return True

    def instantiate(self, selector: SelectorLike) -> None:
        """ force set values:
        - set values to brick size"""
        if self._element is None:
            raise RuntimeError("BrickStairSampler requires an element to instantiate")
        structure_parameters = self._element.values.parameters.structure_parameters

        my_default= {
            "nr_steps"  : 15,
            "default_u" : [ 0.0, 1.0, 0.0],
            "default_v" : [ 0.0, 0.0, 1.0],
            "default_h": 0.5,
            "default_x1": 1.0,
            "default_x2": 1.0,
            "func": [
                "p = [ t * 10.0, t * 5.0, 0.0 ]"
            ]
        }
        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""
        if self._element is None:
            raise RuntimeError("BrickStairSampler requires an element to generate")
        structure_private = self._element.values.parameters.structure_private

        # create main object
        parent = concrete.add_child(None, "parent")

        nr = structure_private["nr_steps"]
        default_u = cgtypes.vec3(structure_private["default_u"])
        default_h = structure_private["default_h"]
        default_x1 = structure_private["default_x1"]
        default_x2 = structure_private["default_x2"]
        func_array = structure_private["func"]
        func = functools.reduce(lambda a, b: a + "\n" + b, func_array )
        values = []

        for n in range(0, nr + 1): # need to go beyond 1
            value = {}
            locals={
                "n":n,
                "t":float(n)/float(nr),
                "p":[],
                "x1":default_x1,
                "x2":default_x2,
                "h":default_h,
                "u":default_u,
                }
            globals={"math":math}
            exec(func, globals, locals)
            value["p"]  = cgtypes.vec3(locals["p"])
            value["u"]  = cgtypes.vec3(locals["u"])
            value["v"]  = cgtypes.vec3(locals["v"]) if "v" in locals else None
            value["h"] = locals["h"]
            value["x1"] = locals["x1"]
            value["x2"] = locals["x2"]
            values.append(value)

        points = []
        points_index = []
        index = 0
        for n in range(0, nr): # compute all values, including upper stairs step needing next value
            # start with "v", as it's needed for step
            value = values[n]
            value_next = values[n + 1]
            point_index = {}
            if value["v"] is None:
                diff = value_next["p"] - value["p"]
                diff_vec3 = diff.normalize()
                value["v"] = (diff_vec3.cross(value['u'])).normalize()
                logger.debug(f'diff {diff_vec3} v={value["v"]}')

            value["v"] = value["v"].normalize()

            u0 = value["p"]*value["u"]
            u1 = value_next["p"]*value["u"]
            h = u1-u0

            def append_point(index, n, p):
                point_index[n] = index
                points.append(p)
                index = index + 1
                return (index,p)

            (index, p00) = append_point(index, "0-0", value["p"] - value["x1"] * value["v"])
            (index, _) =append_point(index, "0-D", p00 + (-value["h"]) * value["u"])
            (index, _) =append_point(index, "0-U", p00 + h * value["u"])
            (index, p11) = append_point(index, "1-0", value["p"] + value["x2"] * value["v"])
            (index, _) =append_point(index, "1-D", p11 + (-value["h"]) * value["u"])
            (index, _) =append_point(index, "1-U", p11 + h * value["u"])
            points_index.append(point_index)

        index = parent.add_structure_points(points)

        table = []

        for i in range(0, nr -1):
            # horiz step
            table.append([
                    points_index[i]["0-0"],
                    points_index[i]["1-0"],
                    points_index[i]["1-U"],
                    points_index[i]["0-U"]
            ])
            # vert step
            table.append([
                    points_index[i]["0-U"],
                    points_index[i]["1-U"],
                    points_index[i+1]["1-0"],
                    points_index[i+1]["0-0"]
            ])
            # side 0
            table.append([
                    points_index[i]["0-D"],
                    points_index[i]["0-U"],
                    points_index[i+1]["0-0"],
                    points_index[i+1]["0-D"]
            ])
            # side 1
            table.append([
                    points_index[i+1]["1-D"],
                    points_index[i+1]["1-0"],
                    points_index[i]["1-U"],
                    points_index[i]["1-D"]
            ])
            # lower
            table.append([
                    points_index[i]["0-D"],
                    points_index[i+1]["0-D"],
                    points_index[i+1]["1-D"],
                    points_index[i]["1-D"]
            ])


        parent.add_structure_faces(
                    index,
                    table,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_GROUND, concrete_room.Node.HINT_BUILDING],
                    {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )



register_brick_type(BrickStairSampler())
