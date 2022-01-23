"""
structure definition for a simple pads provider brick
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

logger  =logging.getLogger()
logger.setLevel(logging.INFO)

from .register import register_brick_type

from jsonmerge import merge

class BrickSimplePadProvider(BrickStructure):

    _name = "simple_pad_provider"

    def __init__(self, _element=None):
        """ init brick """
        self._element = _element

    def get_instance(self, brick:None):
        """Return an instante"""
        return BrickSimplePadProvider(brick)

    def check_structure(self):
        """check everything is as expected.
        """
        logging.info("checking is ok")
        return True

    def instantiate(self, selector):
        """ Use as-is"""
        structure_parameters = self._element.values.parameters.structure_parameters
        my_default= {}
        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)
        structure_private = self._element.values.parameters.structure_private
        logging.info("private: %s", str(structure_private))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""

        pads = self._element.values.pads
        for pad in pads:
            logger.info("Generate pad:%s", pad.pad_id)
            pos = pad.definition.position
            pad_mat = cgtypes.mat4(
                        1.0, 0.0, 0.0, pos[0],
                        0.0, 1.0, 0.0, pos[1],
                        0.0, 0.0, 1.0, pos[2],
                        0.0, 0.0, 0.0, 1.0
                        )
            concrete.add_child(None, pad.pad_id, pad_mat)


register_brick_type(BrickSimplePadProvider())
