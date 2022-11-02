"""
structure definition for a simple pads provider brick
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

logger = logging.getLogger("simple_pad_provider")
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
        logger.debug("checking is ok")
        return True

    def instantiate(self, selector):
        """ Use as-is"""
        structure_parameters = self._element.values.parameters.structure_parameters
        my_default= {}
        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)
        structure_private = self._element.values.parameters.structure_private
        logger.debug("private: %s", str(structure_private))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""

        pads = self._element.values.pads
        for pad in pads:
            pad_mat = cgtypes.mat4(1.0)
            if pad.definition:
                if pad.definition.translation:
                    pos = pad.definition.translation
                    pad_mat = pad_mat * cgtypes.mat4(
                        1.0, 0.0, 0.0, pos[0],
                        0.0, 1.0, 0.0, pos[1],
                        0.0, 0.0, 1.0, pos[2],
                        0.0, 0.0, 0.0, 1.0)
                if pad.definition.rotation:
                    r = pad.definition.rotation
                    if r.angle and r.axis:
                        pad_mat = pad_mat * cgtypes.mat4.rotation(r.angle, r.axis)
            concrete.add_child(None, pad.pad_id, pad_mat)


register_brick_type(BrickSimplePadProvider())
