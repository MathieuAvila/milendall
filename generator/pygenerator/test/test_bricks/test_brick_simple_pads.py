"""
test rectangular room
"""

import logging
import sys
import pathlib
import cgtypes.vec3

import unittest
import bricks.simple_pads as simple_pads
import room
import brick
import concrete_room
import selector_regular
import dressings.basic as basic
import math

logger = logging.getLogger("TestBrick_SimplePad")
logger.level = logging.DEBUG

def_0 = {
   "b_id": "b0",
   "pads": [
       {
            "pad_id" : "p0"
       },
       {
            "pad_id" : "p1",
            "definition" : {
               "translation" : [ 1.0, 2.0, 3.0]
            }
       },
       {
            "pad_id" : "p2",
            "definition" : {
                "rotation" : {
                   "axis" : [ 0.0, 1.0, 0.0],
                   "angle" : math.pi
                }
           }
       },
       {
            "pad_id" : "p3",
            "definition" : {
                "translation" : [ 4.0, 5.0, 6.0],
                "rotation" : {
                   "axis" : [ 0.0, 1.0, 0.0],
                   "angle" : math.pi
                }

           }
       }
    ],
   "parameters": {
       "structure_class": "simple_pad_provider"
   }
  }

class TestBrick_SimplePad(unittest.TestCase):

    def generate(self):
        selector = selector_regular.SelectorRegular()
        custom_brick = brick.Brick(def_0, selector)
        custom_brick.structure_personalization()
        custom_brick.dressing_instantiation()
        custom_brick.dressing_personalization()
        concrete = concrete_room.ConcreteRoom()
        custom_brick.finalize(concrete)
        return concrete

    def test_has_4_pads(self):
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        concrete = self.generate()

        objects = concrete.get_objects()
        self.assertEquals(len(objects), 4)

    def test_void_pad(self):
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        concrete = self.generate()

        object = concrete.get_objects()[0]
        p_org = cgtypes.vec3(10.0, 11.0, 12.0)
        p0 = object.matrix * p_org
        self.assertEqual(p0, p_org)

    def test_void_translation(self):
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        concrete = self.generate()

        object = concrete.get_objects()[1]
        p_org = cgtypes.vec3(10.0, 11.0, 12.0)
        p0 = object.matrix * p_org
        self.assertEqual(p0, (p_org + cgtypes.vec3(1.0, 2.0, 3.0)))

    def test_void_rotation(self):
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        concrete = self.generate()

        object = concrete.get_objects()[2]
        p_org = cgtypes.vec3(1.0, 0.0, 0.0)
        p0 = object.matrix * p_org
        length = (p0 - (cgtypes.vec3(-1.0, 0.0, 0.0))).length()
        self.assertAlmostEqual(0.0, length, 2 )

    def test_void_rotation_translation(self):
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        concrete = self.generate()

        object = concrete.get_objects()[3]

        # first check translation
        p_org = cgtypes.vec3(0.0, 0.0, 0.0)
        p0 = object.matrix * p_org
        logger.info(p0)
        self.assertEqual(p0, (p_org + cgtypes.vec3(4.0, 5.0, 6.0)))

        # then both
        p_org = cgtypes.vec3(1.0, 0.0, 0.0)
        p0 = object.matrix * p_org
        logger.info(p0)
        self.assertEqual(p0, cgtypes.vec3(3.0, 5.0, 6.0))

if __name__ == '__main__':
    unittest.main()
