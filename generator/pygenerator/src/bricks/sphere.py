"""
structure definition for a simple sphere brick
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

from .register import register_brick_type

from math import *

from jsonmerge import merge

logger = logging.getLogger("sphere")
logger.setLevel(logging.INFO)

class BrickSphere(BrickStructure):

    _name = "sphere"

    def __init__(self, _element=None):
        """ init brick """
        self._element = _element

    def get_instance(self, brick:None):
        """Return an instante"""
        return BrickSphere(brick)

    def check_fit(self):
        """ Pass the brick, and list of gates, check it can be applied. """
        logger.debug("checking if sphere fits: always ! rectangular rules the world !")
        return 100

    def check_structure(self):
        """check everything is as expected.
        """
        logger.debug("checking if sphere is ok: always ! sphere rules the world !")
        return True

    def instantiate(self, selector):
        """ force set values:
        - set values to brick size"""

        structure_parameters = self._element.values.parameters.structure_parameters
        my_default = {
            "setup": {
                "radius": 40,
                "lats" : 50,
                "longs" : 50,
                "internal" : True, # by default, build a sky sphere.
                "hard" : False # If this needs to be walkable
            }
        }
        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)
        logger.debug("setup: %s", str(self._element.values.parameters.structure_private["setup"]))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.parameters.structure_private


        setup = structure_private["setup"]

        self.lats = setup["lats"]
        self.longs = setup["longs"]
        R = setup["radius"]
        internal = -1.0 if setup["internal"] else 1.0
        phys = {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} if setup["hard"] else {}

        parent = concrete.add_child(None, "parent")

        points = []
        points_index = []
        index = 0

        for i in range(0, self.lats + 1):
            lat = pi * (-0.5 + float(float(i) / float(self.lats)))
            z1 = sin(lat)
            zr1 = cos(lat)

            line = []

            for j in range(0, self.longs + 1 ):
                lng = 2 * pi * float(float(j+1) / float(self.longs))
                x = cos(lng)
                y = sin(lng)

                point = cgtypes.vec3(x * zr1 * R, y * zr1 * R , internal * z1 * R)
                points.append(point)
                line.append(index)

                index = index + 1

            points_index.append(line)

        index_wall = parent.add_structure_points(points)

        table = []
        table_sky = []
        for i in range(1, self.lats - 1):
            for j in range(0, self.longs):
                table.append([
                    points_index[i][j],
                    points_index[i][j+1],
                    points_index[i+1][j+1],
                    points_index[i+1][j]]
                )
        elem0 = []
        elemN = []
        for j in range(0, self.longs):
                elem0.append(
                    points_index[self.lats -1][j])
                elemN.append(
                    points_index[1][self.longs - j]
                )

        table.append(elem0)
        table.append(elemN)
        parent.add_structure_faces(
                    index_wall,
                    table,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                    phys )



register_brick_type(BrickSphere())
