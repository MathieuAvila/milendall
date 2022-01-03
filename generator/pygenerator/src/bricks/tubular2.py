"""
structure definition for a simple rectangular room
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4
import math

from .register import register_room_type

from math import *

from jsonmerge import merge

class TubularRoom(BrickStructure):

    _name = "tubular2"

    def __init__(self, _element=None):
        """ init room """
        self._element = _element

    def get_instance(self, room:None):
        """Return an instante"""
        return TubularRoom(room)

    def check_fit(self):
        """ Pass the Room, and list of gates, check it can be applied. """
        logging.info("checking if tubular fits: only if exactly 2 gates")
        if self._element.gates.len() == 2:
            return 200
        else:
            return 0

    def check_structure(self):
        """check everything is as expected.
        """
        logging.info("checking if tubular is ok. Only if 2 gates")
        return len(self._element.gates) == 2

    def instantiate(self, selector):
        """ force set values:
        - set values to room size
        - set values for gates"""

        structure_parameters = self._element.values.structure_parameters

        my_default = {
            "segments_nr": 5,                                 # 5 segments
            "func": [
                "values = [",
                "[0.0, 0.0, float(t) ],",
                "[1.6, 0.0, float(t) ],",
                "[1.6, 1.6, float(t) ],",
                "[0.0, 1.6, float(t) ],",
                "]" ],
            "mode_up_gravity" : "default",
            "gates" : []  # stores the order of the 2 gates
        }
        counter = 0
        for gate in self._element.gates:
            my_default["gates"].append(gate.get_id())

        self._element.values.structure_private = merge( my_default, structure_parameters)
        logging.info("setup: %s", str(self._element.values.structure_private))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.structure_private

        setup = structure_private
        gates = structure_private["gates"]
        parent = concrete.add_child(None, "parent")

        points = []
        segments = []

        index = 0

        my_func = "\n".join(setup["func"])
        print(my_func)

        for i in range(0, setup["segments_nr"]):

            t = float(i)
            values=[]
            locals={"t":t, "values":[]}
            globals={"math":math}
            exec(my_func, globals, locals)
            values=locals["values"]
            segment = []
            print(values)
            for val in values:
                p = cgtypes.vec3(val[0], val[1], val[2])
                points.append(p)
                segment.append(index)
                index = index + 1

            segments.append(segment)

        index_wall = parent.add_structure_points(points)

        table_ground = []

        # Nr of elements in segment
        count_seg = len(segments[0])
        # this one will store the default orientation in order to build a default gravity / up vector list
        directions =  []
        for i in range(0, setup["segments_nr"]-1):
            for j in range(0, count_seg):
                table_ground.append([
                    segments[i+1]   [j],
                    segments[i+1]   [(j+1) % count_seg],
                    segments[i] [(j+1) % count_seg],
                    segments[i] [j]])
        # compute main direction of segment, for later use if needed
        for i in range(0, setup["segments_nr"]-1):
            pO = points[segments[i][0]]
            pX = points[segments[i][1]]
            pY = points[segments[i+1][0]]
            vX = pX-pO
            vY = pY-pO
            middle = pO + vX/2.0 + vY/2.0
            up = (vX.cross(vY)).normalize()
            new_dir = {"middle":middle, "up": up}
            #print(new_dir)
            directions.append(new_dir)

        parent.add_structure_faces(
                    index_wall,
                    table_ground,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_GROUND, concrete_room.Node.HINT_BUILDING],
                    {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        if setup["mode_up_gravity"] == "follow":
            middle_list = []
            up_list = []
            for d in directions:
                middle_list.append(("{%s,%s,%s}\n" % (d["middle"].x, d["middle"].y, d["middle"].z)))
                up_list.append(("{%s,%s,%s}\n" % (d["up"].x, d["up"].y, d["up"].z)))
            up = "{ " + ", ".join(up_list) + " }"
            middle = "{ " + ", ".join(middle_list) + " }"
            print(middle)

            parent.set_gravity_script(
            'function optimize_gravity(x, y, z, middle_list, up_list, count, grav_factor)\n' +
            'local tab_out = {}\n' +
            'local dist = 100000000\n' +
            'local current = 1\n' +
            'for d = 1, count-1 do\n' +
            '    local m = middle_list[d]\n' +
            '    local l_dist = (m[1]- x)^2 + (m[2]- y)^2 + (m[3]- z)^2\n' +
            '    if (l_dist < dist) then\n' +
            '        current = d\n' +
            '        dist = l_dist\n' +
                'end\n' +
            'end\n' +
            'tab_out["x"] = up_list[current][1] * grav_factor\n' +
            'tab_out["y"] = up_list[current][2] * grav_factor\n' +
            'tab_out["z"] = up_list[current][3] * grav_factor\n' +
            'tab_out["u_x"] = -up_list[current][1]\n' +
            'tab_out["u_y"] = -up_list[current][2]\n' +
            'tab_out["u_z"] = -up_list[current][3]\n' +
            'tab_out["v"] = 0.01\n' +
            'return tab_out\n' +
            'end\n' +
            '   local grav_factor = %s\n' % setup["gravity_factor"] +
            '   local middle_list = %s\n' % middle +
            '   local up_list = %s\n' % up +
            '   local optim_grav = optimize_gravity(tab_in["x"], tab_in["y"], tab_in["z"], middle_list, up_list, %s, grav_factor)\n' % setup["segments_nr"] +
            '   tab_out["g_x"] = optim_grav["x"]\n' +
            '   tab_out["g_y"] = optim_grav["y"]\n' +
            '   tab_out["g_z"] = optim_grav["z"]\n' +
            '   tab_out["u_x"] = optim_grav["u_x"]\n' +
            '   tab_out["u_y"] = optim_grav["u_y"]\n' +
            '   tab_out["u_z"] = optim_grav["u_z"]\n' +
            '   tab_out["v"] = 0.01\n', [ concrete_room.Node.GRAVITY_SIMPLE ])

        is_first = True
        for gate_id in gates:
            if self._element.gates[0].get_id() == gate_id:
                gate = self._element.gates[0]
            elif self._element.gates[1].get_id() == gate_id:
                gate = self._element.gates[1]
            else:
                raise "unknown gate " + gate_id
            # compute rotation argument depending on room is gate in or out.
            # This rotates locally gate sub object in order to present correct face.
            is_in = 1
            dims = gate.get_dimensions()
            if gate.values.connect[0] == self._element.values.room_id:
                is_in = -1
            if is_first:
                elem = 0
            else:
                is_in = - is_in
                elem = setup["segments_nr"] - 1
            logging.info("Adding gate child, gate %s, connect %s - is_in %s",
                gate_id, gate.values.connect, is_in)
            org = points[segments[elem][0]]
            org2 = points[segments[elem][1]]
            org3 = points[segments[elem][count_seg-1]]
            X = (org2 - org).normalize()
            Y = (org3 - org).normalize()
            print("ORG %s" % org)
            print("X %s" % X)
            print("Y %s" % Y)


            #logging.info("IS_IN:%s, IS_FIRST:%s", str(is_in), str(is_first))

            # create gate object
            gate_mat =  cgtypes.mat4(
                        1.0, 0.0, 0.0, org.x,
                        0.0, 1.0, 0.0, org.y,
                        0.0, 0.0, 1.0, org.z,
                        0.0, 0.0, 0.0, 1.0) * cgtypes.mat4(

                        1.0, 0.0, 0.0, - ( is_in - 1 ) / 2.0 * (dims["portal"][0] + dims["margin"][0]),
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0) * cgtypes.mat4(

                        is_in, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, is_in, 0.0,
                        0.0, 0.0, 0.0, 1.0)
            child_object = concrete.add_child("parent", gate_id, gate_mat)
            is_first = False


register_room_type(TubularRoom())
