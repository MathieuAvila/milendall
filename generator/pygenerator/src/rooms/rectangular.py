"""
structure definition for a simple rectangular room
"""

import logging
import random
from room_structure import RoomStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

from .register import register_room_type

def append_wall(wall_list, width, start_height):
    """Helper: to add a wall to a wall list"""
    if len(wall_list) == 0:
        wall_list.append({"width":width, "start":start_height})
    elif wall_list[-1]["start"] != start_height:
        wall_list.append({"width":width, "start":start_height})
    else:
        wall_list[-1]["width"] += width

def get_sum_walls_width(wall_list):
    """Helper: Get the sum of all width of the walls"""
    return sum([ w["width"] for w in wall_list ])

class RectangularRoom(RoomStructure):

    _name = "rectangular"

    def __init__(self, _element=None):
        """ init room """
        self._element = _element

    def get_instance(self, room:None):
        """Return an instante"""
        return RectangularRoom(room)

    def check_fit(self):
        """ Pass the Room, and list of gates, check it can be applied. """
        logging.info("checking if rectangular fits: always ! rectangular rules the world !")
        return True

    def check_structure(self):
        """check everything is as expected.
        """
        logging.info("checking if rectangular is ok: always ! rectangular rules the world !")
        return True

    def instantiate(self):
        """ force set values:
        - set values to room size
        - set values for gates"""

        structure_private = self._element.values.structure_private

        # instantiate gate acceptable pre and post range
        if "gate_pre_post_range" not in structure_private:
            # for each gate, min and max space before and after
            # [ [pre min, pre max] [post min, post_max] ]
            structure_private["gate_pre_post_range"] = [ [ 1.0 , 1.0 ] , [ 1.0 , 1.0 ] ]
        gate_pre_post_range = structure_private["gate_pre_post_range"]

        # instantiate ceiling height range over highest gate
        # min and max height to add to hignest gate
        if "height_over_gate_range" not in structure_private:
            structure_private["height_over_gate_range"] = [ 1.0 , 1.0 ]

        # instantiate ceiling height over highest gate
        # this is the real one added to the highest gate
        # whatever the number and height of gates, a minimum of height of 2.0 will be taken
        if "height_over_gate" not in structure_private:
            height_range = structure_private["height_over_gate_range"]
            structure_private["height_over_gate"] = random.randint(height_range[0], height_range[1])

        # instantiate range for walls pre-post
        if "wall_pre_post_range" not in structure_private:
            # for each wall, min and max space before and after. It adds to gates.
            # [ [pre min, pre max] [post min, post_max] ]
            structure_private["wall_pre_post_range"] = [ [ 2.0 , 2.0 ] , [ 2.0 , 2.0 ] ]
        wall_pre_post_range = structure_private["wall_pre_post_range"]

        # instantiate wall pre/post
        if "wall_pre_post" not in structure_private:
            structure_private["wall_pre_post"] = [
                [random.randint(wall_pre_post_range[0][0], wall_pre_post_range[0][1]),
                 random.randint(wall_pre_post_range[1][0], wall_pre_post_range[1][1])
                ] for i in range(0,4)
            ]

        # instantiate gates position
        if "setup" not in structure_private:
            structure_private["setup"] = [ [],[],[],[],[] ] # x0, x1, y0, y1
            setup = structure_private["setup"]
            for gate in self._element.gates:
                pos = random.randint(0, 3)
                pre = random.randint(gate_pre_post_range[0][0], gate_pre_post_range[0][1])
                post =  random.randint(gate_pre_post_range[1][0], gate_pre_post_range[1][1])
                setup[pos].append({"gate": gate.get_id(), "pre":pre, "post":post})

        logging.info("setup: %s", str(structure_private["setup"]))
        logging.info("height_over_gate_range: %s", str(structure_private["height_over_gate_range"]))
        logging.info("height_over_gate: %s", str(structure_private["height_over_gate"]))
        logging.info("wall_pre_post_range: %s", str(structure_private["wall_pre_post_range"]))
        logging.info("wall_pre_post: %s", str(structure_private["wall_pre_post"]))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.structure_private
        wall_pre_post = structure_private["wall_pre_post"]
        setup = structure_private["setup"]

        wall_required_size = []
        minimum_gate_height = 3.0

        # for each wall, account required size, and build wall list
        walls = [] # 1 for each direction, and a sublist of [width, height_start]
        gates = [] # 1 for each direction, containing reference
        for i in range(0,4):
            walls.append([])
            wall_list = walls[i]
            gates.append([])
            gates_list = gates[i]
            append_wall(wall_list, wall_pre_post[i][0], 0)
            for gate_def in setup[i]:
                gate_id = gate_def["gate"]
                logging.info("direction %i gate: %s", i, gate_id)
                gate = [g for g in self._element.gates if g.get_id() == gate_id][0]
                dims = gate.get_dimensions()
                # take height into account
                minimum_gate_height = max(minimum_gate_height, dims["portal"][1]+dims["margin"][1])

                # adding pre_gate
                # todo : 0 to be set correctly
                append_wall(wall_list, gate_def["pre"] + dims["margin"][0], 0)
                # at this point, add the gate reference
                gates_list.append({"gate":gate, "gate_id": gate_id, "offset":get_sum_walls_width(wall_list)})
                # adding gate itself
                append_wall(wall_list, dims["portal"][0], dims["portal"][1])
                # adding post-gate
                append_wall(wall_list, gate_def["post"] + dims["margin"][0], 0)

            append_wall(wall_list, wall_pre_post[i][1], 0)
            wall_required_size.append(get_sum_walls_width(wall_list))
            logging.info("direction %i walls: %s", i, wall_list)

        # compute height
        height_over_gate = structure_private["height_over_gate"]
        height = minimum_gate_height + height_over_gate

        # for each direction, account required size based on wall required size
        direction_size = [
            max(wall_required_size[0], wall_required_size[1]),
            max(wall_required_size[2], wall_required_size[3])
        ]

        setup = structure_private["setup"]
        logging.info("wall_required_size: %s" , str(wall_required_size))
        logging.info("direction_size: %s" , str(direction_size))

        # create main object
        parent = concrete.add_child(None, "parent")

        # add extreme points
        index0 = parent.add_structure_points(
            [
                cgtypes.vec3(0,                  0,        0),
                cgtypes.vec3(0,                  height,   0),

                cgtypes.vec3(direction_size[0],  0,        0),
                cgtypes.vec3(direction_size[0],  height,   0),

                cgtypes.vec3(direction_size[0],  0,        direction_size[1]),
                cgtypes.vec3(direction_size[0],  height,   direction_size[1]),

                cgtypes.vec3(0,                  0,       direction_size[1]),
                cgtypes.vec3(0,                  height,  direction_size[1]),
            ])
        # add floor
        parent.add_structure_faces(
            index0,
            [ [6,4,2,0] ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_GROUND, concrete_room.Node.HINT_BUILDING],
            [] )

        # add ceiling
        parent.add_structure_faces(
            index0,
            [ [1,3,5,7] ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_CEILING, concrete_room.Node.HINT_BUILDING],
            [] )

        # add walls in every direction

        wall_matrices = [
            cgtypes.mat4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0),
            cgtypes.mat4(
                -1.0, 0.0, 0.0, direction_size[0],
                0.0, 1.0, 0.0,  0.0,
                0.0, 0.0, -1.0, direction_size[1],
                0.0, 0.0, 0.0, 1.0),
            cgtypes.mat4(
                0.0, 0.0, 1.0, direction_size[0],
                0.0, 1.0, 0.0, 0.0,
                1.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 1.0),
            cgtypes.mat4(
                0.0, 0.0, -1.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                -1.0, 0.0, 0.0, direction_size[1],
                0.0, 0.0, 0.0, 1.0),
        ]
        for wall_dir in range(0,4):
            cdir = int(wall_dir / 2)
            logging.info("wall direction: %i", wall_dir)

            # append pre/post if wall is shorter than direction
            if wall_required_size[wall_dir] < direction_size[cdir]:
                need = direction_size[cdir] - wall_required_size[wall_dir]
                logging.info("shorter: %f %f", wall_required_size[wall_dir], direction_size[cdir])
                append_wall(walls[wall_dir], need, 0)

            wall_mat = wall_matrices[wall_dir]

            offset = 0
            for wall in walls[wall_dir]:
                width = wall["width"]
                index_wall = parent.add_structure_points(
                    [
                    wall_mat*cgtypes.vec4(offset,         wall["start"],   0,1),
                    wall_mat*cgtypes.vec4(offset,         height,       0,1),
                    wall_mat*cgtypes.vec4(offset + width, wall["start"],   0,1),
                    wall_mat*cgtypes.vec4(offset + width, height,       0,1),
                ])
                parent.add_structure_faces(
                    index_wall,
                    [ [2,3,1,0] ],
                    concrete_room.Node.CAT_PHYS_VIS,
                    [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
                    [] )
                offset += width

            # append gates entry
            gates_list = gates[wall_dir]
            logging.info("gates_list %s", gates_list)
            for gate_def in gates_list:
                gate_id = gate_def["gate_id"]
                logging.info("Adding gate child, direction %i gate: %s", wall_dir, dir(gate))
                gate = gate_def["gate"]
                offset = gate_def["offset"]
                # create gate object
                gate_mat = wall_mat* cgtypes.mat4(
                        1.0, 0.0, 0.0, offset,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0)

                child_object = concrete.add_child("parent", gate_id, gate_mat)



register_room_type(RectangularRoom())
