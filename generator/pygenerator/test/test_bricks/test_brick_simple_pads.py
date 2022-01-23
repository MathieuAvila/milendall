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

logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)

logger = logging.getLogger()
logger.level = logging.DEBUG

def_0 = {
   "b_id": "b0",
   "pads": [
       {
           "pad_id" : "p0",
           "definition" : {
               "position" : [ 1.0, 2.0, 3.0]
           }
       },
       {
           "pad_id" : "p1",
           "definition" : {
               "position" : [ 4.0, 5.0, 6.0]
           }
       }
   ],
   "parameters": {
       "structure_class": "simple_pad_provider"
   }
  }

class TestBrick_SimplePad(unittest.TestCase):

    selector = selector_regular.SelectorRegular()

    def test_generate(self):
        """generate one simple door with no parameter"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        selector = selector_regular.SelectorRegular()
        custom_brick = brick.Brick(def_0, selector)
        custom_brick.structure_personalization()
        custom_brick.dressing_instantiation()
        custom_brick.dressing_personalization()
        concrete = concrete_room.ConcreteRoom()
        custom_brick.finalize(concrete)

        # check that it contains an object with expected parent
        objects = concrete.get_objects()
        self.assertEquals(len(objects), 2)

        object = objects[0]
        self.assertEquals(object.parent, None)
        matrix = object.matrix
        p0 = matrix * cgtypes.vec3(0.0)
        self.assertEqual(p0, cgtypes.vec3(1.0,2.0,3.0))

        object = objects[1]
        self.assertEquals(object.parent, None)
        matrix = object.matrix
        p0 = matrix * cgtypes.vec3(0.0)
        self.assertEqual(p0, cgtypes.vec3(4.0,5.0,6.0))

if __name__ == '__main__':
    unittest.main()
