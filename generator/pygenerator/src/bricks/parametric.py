"""
structure definition for a fully parametric brick
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4
import functools
import math

from .register import register_brick_type

from math import *

from jsonmerge import merge

logger = logging.getLogger("parametric")
logger.setLevel(logging.INFO)

class BrickParametric(BrickStructure):

    _name = "parametric"

    def __init__(self, _element=None):
        """ init room """
        self._element = _element

    def get_instance(self, room:None):
        """Return an instante"""
        return BrickParametric(room)

    def check_fit(self):
        """ Pass the Room, check it can be applied. """
        logger.debug("checking if parametric fits: always")
        return 100

    def check_structure(self):
        """check everything is as expected.
        """
        logger.debug("checking if parametric is ok.")
        return True

    def instantiate(self, selector):
        """ force set values:
        - set values to room size"""

        structure_parameters = self._element.values.parameters.structure_parameters

        my_default = {
            "setup": {
                "segments_nr_s": 5,                                 # 5 segments
                "segments_nr_t": 5,                                 # 5 segments
                "func": [
                    "values = [",
                    "s * 10.0, 0.0, t * 10.0, s, t",  # p(s,t) : x,y,z,u,v
                    "]" ]}
        }

        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)
        logger.debug("setup: %s", str(self._element.values.parameters.structure_private))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.parameters.structure_private

        setup = structure_private["setup"]
        parent = concrete.add_child(None, "parent")

        points = []
        indexes = []
        counter = 0

        nr_s = setup["segments_nr_s"]
        nr_t = setup["segments_nr_t"]
        func_array = setup["func"]
        func = functools.reduce(lambda a, b: a + "\n" + b, func_array )

        for i in range(0, nr_s + 1):
            s = float(i) / float(nr_s)
            index_raw = []
            for j in range(0, nr_t + 1):
                t = float(j) / float(nr_t)
                locals={"s":s, "t":t, "values":[]}
                globals={"math":math}
                exec(func, globals, locals)
                values=locals["values"]
                if len(values) != 5:
                    raise "func must return a table of 5"
                segment = []
                p = cgtypes.vec3(values[0], values[1], values[2])
                p.u = values[3]
                p.v = values[4]
                p.has_tex = True
                index_raw.append(counter)
                points.append(p)
                counter = counter + 1
            indexes.append(index_raw)

        index_wall = parent.add_structure_points(points)

        table = []

        for i in range(0, nr_s):
            points_raw = []
            for j in range(0, nr_t):
                table.append([
                    indexes[i]   [j+1],
                    indexes[i+1] [j+1],
                    indexes[i+1] [j],
                    indexes[i]   [j]])

        parent.add_structure_faces(
                    0,
                    table,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_GROUND, concrete_room.Node.HINT_BUILDING],
                    {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )




register_brick_type(BrickParametric())
