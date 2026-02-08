"""
structure definition for a simple rectangular room
"""

from __future__ import annotations

import logging
import math

import concrete_room
import gltf_helper
from cgtypes.vec3 import vec3
from dressing import Dressing
from .register import register_dressing_type

from merge_utils import merge
from typing_defs import ElementWithValues, SelectorLike

logger = logging.getLogger("basic")
logger.setLevel(logging.INFO)

def find_texture(point: vec3) -> vec3:
    h_angle = math.atan2(point.x, point.y)
    length = math.sqrt( point.x * point.x +  point.y * point.y +  point.z * point.z )
    p = vec3( -(h_angle + math.pi) / math.pi, (point.z + length)  / (2*length) )
    return p

class DressingSphere(Dressing):

    _name = "sphere"

    element: ElementWithValues | None

    def __init__(self, element: ElementWithValues | None = None) -> None:
        """ init dressing """
        #super().__init__(element)
        self.element = element

    def check_fit(self) -> int:
        """ Check dressing fits. """

        if self.element is not None and self.element.values.parameters.structure_class == "sphere":
            return 100
        else:
            return 0

    def get_instance(self, element: ElementWithValues | None = None) -> DressingSphere:
        """ return a self instance of this dressing, operating on any object"""
        return DressingSphere(element)

    def instantiate(self, selector: SelectorLike) -> None:
        """ force set values:
        - set default values to dressing"""
        if self.element is None:
            raise RuntimeError("DressingSphere requires an element to instantiate")
        self.element.values.parameters.dressing_private = {}
        dressing_parameters = self.element.values.parameters.dressing_parameters
        my_default = {
            "setup": [
                [concrete_room.Node.HINT_GROUND, "/common/planets/earth.jpg" ],
                [concrete_room.Node.HINT_WALL, "/common/planets/earth.jpg" ],
                [concrete_room.Node.HINT_CEILING, "/common/planets/milky_way.jpg" ]
            ]
        }
        self.element.values.parameters.dressing_private = merge(my_default, dressing_parameters)
        logger.debug("setup: %s", str(self.element.values.parameters.dressing_private["setup"]))

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""

        if self.element is None:
            raise RuntimeError("DressingSphere requires an element to generate")
        dressing_private = self.element.values.parameters.dressing_private
        if dressing_private is None:
            raise RuntimeError("DressingSphere requires dressing parameters to generate")

        setup = dressing_private.get("setup")
        if not isinstance(setup, list):
            raise RuntimeError("DressingSphere requires setup entries")

        for obj in concrete.get_objects():
            logger.debug("treating object: %s", obj.name)

            # get each kind of walls and associate a texture.

            for kind_texture, texture in setup:
                list_faces = obj.get_visual_face([kind_texture])
                logger.debug("kind: %s, texture:%s , len:%i",
                            kind_texture, texture, len(list_faces))
                if len(list_faces) != 0:
                    for faces in list_faces:
                        faces_list = faces["faces"]
                        if not isinstance(faces_list, list):
                            continue
                        obj.add_dressing_faces(
                            obj.structure_points,
                            faces_list,
                            gltf_helper.get_texture_definition_with_map(texture, find_texture))

register_dressing_type(DressingSphere())
