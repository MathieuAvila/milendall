"""
structure definition for a parametric stair brick
"""

from __future__ import annotations

import functools
import logging
import math

from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

import milendall_math
import gltf_helper

from .register import register_brick_type

from jsonmerge import merge
from typing_defs import ElementWithValues, SelectorLike

logger = logging.getLogger("object_sampler")
logger.setLevel(logging.INFO)

class BrickObjectSampler(BrickStructure):

    _name = "object_sampler"

    _element: ElementWithValues | None

    def __init__(self, _element: ElementWithValues | None = None) -> None:
        """ init brick """
        self._element = _element

    def get_instance(self, brick: ElementWithValues) -> BrickObjectSampler:
        """Return an instance"""
        return BrickObjectSampler(brick)

    def check_structure(self) -> bool:
        """check everything is as expected.
        """
        logger.debug("checking if stair is ok")
        return True

    def instantiate(self, selector: SelectorLike) -> None:
        """ force set values:
        - set values to brick size"""
        if self._element is None:
            raise RuntimeError("BrickObjectSampler requires an element to instantiate")
        structure_parameters = self._element.values.parameters.structure_parameters

        my_default= {
            "nr"  : 3,
            "func": [
                "p = [ n * 1.0, 0.5, 0.5 ]",
                "u = [ 0.0, 1.0, 0.0]",
                "o = {'subtype' : 'time_+1'}",
                "t = 'option'"
            ]
        }
        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""
        if self._element is None:
            raise RuntimeError("BrickObjectSampler requires an element to generate")
        structure_private = self._element.values.parameters.structure_private

        # create main object
        parent = concrete.add_child(None, "parent")

        nr = structure_private["nr"]
        func_array = structure_private["func"]
        func = functools.reduce(lambda a, b: a + "\n" + b, func_array )

        for n in range(0, nr): # need to go beyond 1
            value = {}
            locals={
                "n":n,
                "t":"",
                "p":[],
                "u":[],
                "o": {}
                }
            globals={"math":math}
            exec(func, globals, locals)
            parent.add_object(locals["t"], locals["p"], parameters = locals["o"])

register_brick_type(BrickObjectSampler())
