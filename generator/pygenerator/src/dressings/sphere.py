"""
structure definition for a simple rectangular room
"""

import logging

import concrete_room
from dressing import Dressing
from .register import register_dressing_type
import cgtypes.vec3
import math

from jsonmerge import merge

def find_texture(point):
    h_angle = math.atan2(point.x, point.y)
    length = math.sqrt( point.x * point.x +  point.y * point.y +  point.z * point.z )
    p = cgtypes.vec3( -(h_angle + math.pi) / math.pi, (point.z + length)  / (2*length) )
    return p

class DressingSphere(Dressing):

    _name = "sphere"

    def __init__(self, element=None):
        """ init dressing """
        #super().__init__(element)
        self.element = element

    def check_fit(self):
        """ Check dressing fits. """

        if self.element.values.structure_class == "sphere":
            return 100
        else:
            return 0

    def get_instance(self, element:None):
        """ return a self instance of this dressing, operating on any object"""
        return DressingSphere(element)

    def instantiate(self, selector):
        """ force set values:
        - set default values to dressing"""
        self.element.values.dressing_private={}
        dressing_parameters = self.element.values.dressing_parameters
        my_default = {
            "setup": [
                [concrete_room.Node.HINT_GROUND, "/common/planets/earth.jpg" ],
                [concrete_room.Node.HINT_WALL, "/common/planets/earth.jpg" ],
                [concrete_room.Node.HINT_CEILING, "/common/planets/milky_way.jpg" ]
            ]
        }
        self.element.values.dressing_private = merge( my_default, dressing_parameters)
        logging.info("setup: %s", str(self. element.values.dressing_private["setup"]))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""

        dressing_private = self.element.values.dressing_private

        for obj in concrete.get_objects():
            logging.info("treating object: %s", obj.name)

            # get each kind of walls and associate a texture.

            for kind_texture, texture in dressing_private["setup"]:
                list_faces = obj.get_visual_face([kind_texture])
                logging.info("kind: %s, texture:%s , len:%i",
                            kind_texture, texture, len(list_faces))
                if len(list_faces) != 0:
                    for faces in list_faces:
                        obj.add_dressing_faces(
                            obj.structure_points,
                            faces["faces"],
                            concrete_room.get_texture_definition_with_map(texture, find_texture))

register_dressing_type(DressingSphere())
