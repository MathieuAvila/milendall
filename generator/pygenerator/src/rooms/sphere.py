"""
structure definition for a simple rectangular room
"""

import logging
from room_structure import RoomStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

from .register import register_room_type

from math import *

from jsonmerge import merge

class SphereRoom(RoomStructure):

    _name = "sphere"

    def __init__(self, _element=None):
        """ init room """
        self._element = _element

    def get_instance(self, room:None):
        """Return an instante"""
        return SphereRoom(room)

    def check_fit(self):
        """ Pass the Room, and list of gates, check it can be applied. """
        logging.info("checking if sphere fits: always ! rectangular rules the world !")
        return 100

    def check_structure(self):
        """check everything is as expected.
        """
        logging.info("checking if sphere is ok: always ! sphere rules the world !")
        return True

    def instantiate(self, selector):
        """ force set values:
        - set values to room size
        - set values for gates"""

        structure_parameters = self._element.values.structure_parameters
        my_default = {
            "setup": {
                "radius": 20,
                "lats" : 50,
                "longs" : 50
            }
        }
        self._element.values.structure_private = merge( my_default, structure_parameters)
        logging.info("setup: %s", str(self._element.values.structure_private["setup"]))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.structure_private


        setup = structure_private["setup"]
        #child_object = concrete.add_child("parent", gate_id, gate_mat)

        self.lats = setup["lats"]
        self.longs = setup["longs"]
        R = setup["radius"]

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

                point = cgtypes.vec3(x * zr1 * R, y * zr1 * R , z1 * R)
                points.append(point)

                line.append(index)

                index = index + 1

            points_index.append(line)

        index_wall = parent.add_structure_points(points)

        table = []
        for i in range(0, self.lats):
            for j in range(0, self.longs):
                table.append([
                    points_index[i][j],
                    points_index[i][j+1],
                    points_index[i+1][j+1],
                    points_index[i+1][j]]
                )

        parent.set_gravity({})
        parent.set_gravity_script(
            '	tab_out["g_x"] = -tab_in["x"]*0.001\n'
	        '   tab_out["g_y"] = -tab_in["y"]*0.001\n'
	        '   tab_out["g_z"] = -tab_in["z"]*0.001\n'
        	'   tab_out["u_x"] = tab_in["x"]\n'
	        '   tab_out["u_y"] = tab_in["y"]\n'
	        '   tab_out["u_z"] = tab_in["z"]\n'
        	'   tab_out["v"] = 0.01\n', [ concrete_room.Node.GRAVITY_SIMPLE ])
        parent.add_structure_faces(
                    index_wall,
                    table,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                    {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )



register_room_type(SphereRoom())
