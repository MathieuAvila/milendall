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
            },
            "gates" : {}
        }
        counter = 0
        for gate in self._element.gates:
            v_angle = pi/4 * counter
            h_angle = pi/4 * ( counter % 8 )
            my_default["gates"][gate.get_id()]= { "v_angle":v_angle, "h_angle":h_angle}
            counter = counter + 1

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
        parent.set_gravity({})
        parent.set_gravity_script(
            '   local grav_factor\n'
            '   grav_factor = 30 * %s\n'
            '   tab_out["g_x"] = -tab_in["x"] / grav_factor \n'
            '   tab_out["g_y"] = -tab_in["y"] / grav_factor \n'
            '   tab_out["g_z"] = -tab_in["z"] / grav_factor \n'
            '   tab_out["u_x"] = tab_in["x"]\n'
            '   tab_out["u_y"] = tab_in["y"]\n'
            '   tab_out["u_z"] = tab_in["z"]\n'
            '   tab_out["v"] = 0.01\n' % R , [ concrete_room.Node.GRAVITY_SIMPLE ])
        parent.add_structure_faces(
                    index_wall,
                    table,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                    {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        for gate in self._element.gates:
            gate_id = gate.get_id()
            # compute rotation argument depending on room is gate in or out.
            # This rotates locally gate sub object in order to present correct face.
            is_in = 1
            dims = gate.get_dimensions()
            if gate.values.connect[0] == self._element.values.room_id:
                is_in = -1
            logging.info("Adding gate child, gate %s, connect %s - is_in %s",
                gate.values.gate_id, gate.values.connect, is_in)
            # create gate object
            def_gate = structure_private["gates"][gate_id]
            gate_mat = cgtypes.mat4.rotation(
                def_gate["h_angle"], [0,0,1]) * cgtypes.mat4.rotation(
                    def_gate["v_angle"], [1,0,0]) * cgtypes.mat4(
                        1.0, 0.0, 0.0, -( dims["portal"][0]) / 2.0,
                        0.0, 1.0, 0.0, R - 0.1,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0
                        )


            child_object = concrete.add_child("parent", gate_id, gate_mat)




register_room_type(SphereRoom())
