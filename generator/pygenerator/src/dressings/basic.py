"""
structure definition for a simple rectangular room
"""

import logging

import concrete_room
from dressing import Dressing
from .register import register_dressing_type

class DressingBasic(Dressing):

    _name = "basic"

    def __init__(self, element=None):
        """ init dressing """
        #super().__init__(element)
        self.element = element

    def get_instance(self, element:None):
        """ return a self instance of this dressing, operating on any object"""
        return DressingBasic(element)

    def instantiate(self, selector):
        """ force set values:
        - set default values to dressing"""
        self.element.values.dressing_private={}

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        for obj in concrete.get_objects():
            logging.info("treating object: %s", obj.name)

            # get each kind of walls and associate a texture.

            for kind_texture, texture, axes in [
                [concrete_room.Node.HINT_GROUND, "/common/basic/ground.jpg", [["x"], ["z"]] ],
                [concrete_room.Node.HINT_WALL, "/common/basic/wall.jpg",  [["x","z"], ["y"]] ],
                [concrete_room.Node.HINT_CEILING, "/common/basic/ceiling.jpg", [["x"], ["z"]] ],
                [concrete_room.Node.HINT_DOOR, "/common/basic/door.jpg", [["x"], ["z"]] ]
                ]:
                list_faces = obj.get_visual_face([kind_texture])
                logging.debug("kind: %s, texture:%s , len:%i",
                            kind_texture, texture, len(list_faces))
                if len(list_faces) != 0:
                    for faces in list_faces:
                        obj.add_dressing_faces(
                            obj.structure_points,
                            faces["faces"],
                            concrete_room.get_texture_definition(texture, axes=axes))

register_dressing_type(DressingBasic())
