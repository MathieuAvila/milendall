"""
structure definition for a simple rectangular brick
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

#from gltf_helper import vec4_to_vec3

from .register import register_brick_type

from jsonmerge import merge

class BrickRectangular(BrickStructure):

    _name = "rectangular"

    def __init__(self, _element=None):
        """ init brick """
        self._element = _element

    def get_instance(self, brick:None):
        """Return an instante"""
        return BrickRectangular(brick)

    def check_structure(self):
        """check everything is as expected.
        """
        logging.info("checking if rectangular is ok: always ! rectangular rules the world !")
        return True

    def instantiate(self, selector):
        """ force set values:
        - set values to brick size
        - set values for gates"""
        structure_parameters = self._element.values.parameters.structure_parameters

        my_default= {
            "size" : [ 5.0 , 2.5, 5.0 ]
        }

        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)
        structure_private = self._element.values.parameters.structure_private

        logging.info("private: %s", str(structure_private))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.parameters.structure_private
        direction_size = structure_private["size"]
        height = direction_size[1]

        # create main object
        parent = concrete.add_child(None, "parent")

        # add extreme points
        index0 = parent.add_structure_points(
            [
                cgtypes.vec3(0,                  0,        0),
                cgtypes.vec3(0,                  height,   0),

                cgtypes.vec3(direction_size[0],  0,        0),
                cgtypes.vec3(direction_size[0],  height,   0),

                cgtypes.vec3(direction_size[0],  0,        direction_size[2]),
                cgtypes.vec3(direction_size[0],  height,   direction_size[2]),

                cgtypes.vec3(0,                  0,       direction_size[2]),
                cgtypes.vec3(0,                  height,  direction_size[2]),
            ])
        # add floor
        parent.add_structure_faces(
            index0,
            [ [6,4,2,0] ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_GROUND, concrete_room.Node.HINT_BUILDING],
            {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        # add ceiling
        parent.add_structure_faces(
            index0,
            [ [1,3,5,7] ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_CEILING, concrete_room.Node.HINT_BUILDING],
            {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        # add walls in every direction
        parent.add_structure_faces(
            index0,
            [ [2,3,1,0], [4,5,3,2], [6,7,5,4], [0,1,7,6] ],
            concrete_room.Node.CAT_PHYS_VIS,
            [concrete_room.Node.HINT_WALL, concrete_room.Node.HINT_BUILDING],
            {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )

        wall_matrices = [
            cgtypes.mat4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                1.0, 1.0, 1.0, 1.0),
            cgtypes.mat4(
                -1.0, 0.0, 0.0, direction_size[0],
                0.0, 1.0, 0.0,  0.0,
                0.0, 0.0, -1.0, direction_size[2],
                1.0, 1.0, 1.0, 1.0),
            cgtypes.mat4(
                0.0, 0.0, 1.0, 0,
                0.0, 1.0, 0.0, 0.0,
                -1.0, 0.0, 0.0, direction_size[2],
                1.0, 1.0, 1.0, 1.0),
            cgtypes.mat4(
                0.0, 0.0, -1.0, direction_size[0],
                0.0, 1.0, 0.0, 0.0,
                1.0, 0.0, 0.0, 0,
                1.0, 1.0, 1.0, 1.0),
        ]
        pads = self._element.values.pads

        for wall_dir in range(0,4):
            # TODO
            wall_mat = wall_matrices[wall_dir]
            # append pads entries
            # TODO



register_brick_type(BrickRectangular())
