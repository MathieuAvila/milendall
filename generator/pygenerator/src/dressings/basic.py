"""
structure definition for a simple dressing
"""

from __future__ import annotations

import logging

import concrete_room
from dressing import Dressing
from .register import register_dressing_type
import gltf_helper

from jsonmerge import merge
from typing_defs import ElementWithValues, SelectorLike

logger = logging.getLogger("basic")
logger.setLevel(logging.INFO)

class DressingBasic(Dressing):

    _name = "basic"

    _element: ElementWithValues | None

    def __init__(self, element: ElementWithValues | None = None) -> None:
        """ init dressing """
        #super().__init__(element)
        self._element = element

    def get_instance(self, element: ElementWithValues | None = None) -> DressingBasic:
        """ return a self instance of this dressing, operating on any object"""
        return DressingBasic(element)

    def instantiate(self, selector: SelectorLike) -> None:
        """ force set values:
        - set default values to dressing"""
        if self._element is None:
            raise RuntimeError("DressingBasic requires an element to instantiate")
        dressing_parameters = self._element.values.parameters.dressing_parameters
        my_default = { "textures": {
            concrete_room.Node.HINT_GROUND : "/common/basic/ground.jpg",
            concrete_room.Node.HINT_WALL : "/common/basic/wall.jpg",
            concrete_room.Node.HINT_CEILING : "/common/basic/ceiling.jpg",
            concrete_room.Node.HINT_DOOR : "/common/basic/door.jpg"
        }}
        self._element.values.parameters.dressing_private = merge(my_default, dressing_parameters)


    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""
        if self._element is None:
            raise RuntimeError("DressingBasic requires an element to generate")
        for obj in concrete.get_objects():
            logger.debug("treating object: %s", obj.name)

            # get each kind of walls and associate a texture.

            axes_map = {concrete_room.Node.HINT_GROUND:  [["x"], ["z"]] ,
                concrete_room.Node.HINT_WALL:  [["x","z"], ["y"]] ,
                concrete_room.Node.HINT_CEILING: [["x"], ["z"]] ,
                concrete_room.Node.HINT_DOOR: [["x"], ["z"]]  }

            private = self._element.values.parameters.dressing_private
            if private is None:
                raise RuntimeError("DressingBasic requires dressing parameters to generate")

            textures = private.get("textures")
            if not isinstance(textures, dict):
                raise RuntimeError("DressingBasic textures are missing")
            for kind_texture in textures.keys():
                texture = textures[kind_texture]
                axes = axes_map[kind_texture]
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
                            gltf_helper.get_texture_definition_function_simple_mapper(texture))

register_dressing_type(DressingBasic())
