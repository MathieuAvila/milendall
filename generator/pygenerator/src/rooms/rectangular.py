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

class RectangularRoom(RoomStructure):

    _name = "rectangular"

    def __init__(self, _element=None):
        """ init room """
        self._element = _element

    def get_instance(self, room:None):
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
                setup[pos].append({"gate": gate, "pre":pre, "post":post})

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
        # for each wall, account required size
        wall_required_size = [ 
                        wall_pre_post[i][0]+wall_pre_post[i][1]
                        + sum([ gate["pre"] + gate["post"] + gate["gate"].size[0] 
                                for gate in setup[i] ])
                        for i in range(0,4)
                    ]

        # compute height
        height_over_gate = structure_private["height_over_gate"]
        minimum_gate_height = max([2.0] ) # todo: add gate height
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
                cgtypes.vec3(0,                0,                0),
                cgtypes.vec3(0,                0,                height),

                cgtypes.vec3(direction_size[0],0,                0),
                cgtypes.vec3(direction_size[0],0,                height),

                cgtypes.vec3(direction_size[0],direction_size[1],0),
                cgtypes.vec3(direction_size[0],direction_size[1],height),

                cgtypes.vec3(0,                direction_size[1],0),
                cgtypes.vec3(0,                direction_size[1],height),
            ])
        # add floor
        parent.add_structure_faces(
            index0,
            [ [0,2,4,6] ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_GROUND],
            [] )

        # add ceiling
        parent.add_structure_faces(
            index0,
            [ [7,5,3,2] ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_CEILING],
            [] )

        # add walls in every direction
        for wall_dir in range(0,4):
            logging.info("wall direction: %i", wall_dir)

        # add gates special places


register_room_type(RectangularRoom())
