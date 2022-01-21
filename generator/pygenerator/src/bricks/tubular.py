"""
structure definition for a simple rectangular room
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

from .register import register_brick_type

from math import *

from jsonmerge import merge

class BrickTubular(BrickStructure):

    _name = "tubular"

    def __init__(self, _element=None):
        """ init room """
        self._element = _element

    def get_instance(self, room:None):
        """Return an instante"""
        return BrickTubular(room)

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
            "setup": {
                "segments_nr": 5,                                 # 5 segments
                "increment": 1.0,                                 # each segment has size 1
                "f_step" : "[ [0.1, 0.0 ], [1.8, 0.0 ], [1.8, 0.0 ], [0.1, 1.6 ] ]", # give position of each segment on Y,Z
                #"f_step" : "[ [0,0], [1.6,0], [1.6,1.8], [0,1.8] ]", # give position of each segment on Y,Z
                "script_up" : {"1.0", "0.0"} ,                    # injected LUA script that gives UP vector on Y,Z
                "script_gravity" :  {"-1.0", "0.0"}               # injected LUA script that gives gravity vector on Y,Z
            },
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

        setup = structure_private["setup"]
        gates = structure_private["gates"]
        parent = concrete.add_child(None, "parent")

        points = []
        segments = []

        index = 0

        for i in range(0, setup["segments_nr"]):

            values = eval(setup["f_step"])
            if len(values) != 4:
                raise "f_step must return a table of 4"
            segment = []
            print(values)
            for j in range(0,4):
                val = values[j]
                p = cgtypes.vec3(val[0], val[1], float(i * setup["increment"]))
                points.append(p)
                segment.append(index)
                index = index + 1

            segments.append(segment)

        index_wall = parent.add_structure_points(points)

        table_ground = []
        table_wall = []
        table_ceiling = []

        for i in range(0, setup["segments_nr"] -1):
            table_ground.append([
                segments[i+1]   [0],
                segments[i+1]   [1],
                segments[i] [1],
                segments[i] [0]])
            table_wall.append([
                segments[i+1]   [1],
                segments[i+1]   [2],
                segments[i] [2],
                segments[i] [1]])
            table_ceiling.append([
                segments[i+1]   [2],
                segments[i+1]   [3],
                segments[i] [3],
                segments[i] [2]])
            table_wall.append([
                segments[i+1]   [3],
                segments[i+1]   [0],
                segments[i] [0],
                segments[i] [3]])


        #parent.set_gravity({})
        #parent.set_gravity_script('   ' % R , [ concrete_room.Node.GRAVITY_SIMPLE ])
        parent.add_structure_faces(
                    index_wall,
                    table_ground,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_GROUND, concrete_room.Node.HINT_BUILDING],
                    {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )
        parent.add_structure_faces(
                    index_wall,
                    table_wall,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                    {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )
        parent.add_structure_faces(
                    index_wall,
                    table_ceiling,
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_CEILING, concrete_room.Node.HINT_BUILDING],
                    {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

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
            if not is_first:
                offset_x = setup["increment"] * setup["segments_nr"]
            org = points[segments[elem][0]]

            #logging.info("IS_IN:%s, IS_FIRST:%s", str(is_in), str(is_first))

            # create gate object
            gate_mat =  cgtypes.mat4(
                        1.0, 0.0, 0.0, 0.0,
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


register_brick_type(BrickTubular())
