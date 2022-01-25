"""
test rectangular room
"""

import logging
import sys
import pathlib

import unittest
import bricks.simple_door as simple_door
import room
import brick
import concrete_room
import selector_regular

import dressings.basic as basic

logger = logging.getLogger("TestBrick_SimpleDoor")
logger.setLevel(logging.INFO)

def_0 = {
   "b_id": "b0",
   "parameters": {
       "structure_parameters" : {
           "gate_id" : "gate0",
            "connect" : "A"
        },
       "structure_class": "simple_door"
   }
  }

class TestBrick_SimpleDoor(unittest.TestCase):

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
        self.assertEquals(len(objects), 1)
        object = objects[0]
        self.assertEquals(object.parent, None)

        # check there is a portal to an outer world (sounds nice, he ?)
        phys_faces = object.get_physical_faces()
        portal_faces = [ faces for faces in phys_faces if object.PHYS_TYPE_PORTAL == faces["physics"]["type"] ]
        self.assertEquals(len(portal_faces), 1)
        portal_face = portal_faces[0]
        self.assertEquals(portal_face["physics"], {'type': 'portal', 'connect': 'A', 'gate' : 'gate0'})

        # won't check face itself, left to visual QC

if __name__ == '__main__':
    unittest.main()
