"""
test rectangular room
"""

from __future__ import annotations

import logging
import sys
import unittest

import brick
import concrete_room
import selector_regular


logger = logging.getLogger("TestBrick_SimpleDoor")
logger.setLevel(logging.INFO)

def_0 = {
   "b_id": "b0",
   "portals" : [
       {
       "gate_id" : "gate0",
       "connect" : "A"
       }
    ],
   "parameters": {
       "structure_class": "simple_door"
   }
  }

class TestBrick_SimpleDoor(unittest.TestCase):

    selector = selector_regular.SelectorRegular()

    def test_generate(self) -> None:
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
        self.assertEqual(len(objects), 1)
        obj = objects[0]
        self.assertEqual(obj.parent, None)

        # check there is a portal to an outer world (sounds nice, he ?)
        phys_faces = obj.get_physical_faces()
        portal_faces = [
            faces for faces in phys_faces
            if obj.PHYS_TYPE_PORTAL == faces["physics"]["type"]
        ]
        self.assertEqual(len(portal_faces), 1)
        portal_face = portal_faces[0]
        self.assertEqual(portal_face["physics"], {'type': 'portal', 'connect': 'A', 'gate' : 'gate0'})

        # won't check face itself, left to visual QC

if __name__ == '__main__':
    unittest.main()
