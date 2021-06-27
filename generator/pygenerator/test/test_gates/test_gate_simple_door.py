"""
test rectangular room
"""

import logging
import sys
import pathlib

import unittest
import gates.simple_door as simple_door
import room
import gate
import concrete_room
import selector_regular

import dressings.basic as basic

logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)

logger = logging.getLogger()
logger.level = logging.DEBUG

def_0 = {
   "gate_id": "gate0",
   "structure_class": "simple_door",
   "connect" : [ "room1", "room2"],
  }

class TestGate_SimpleDoor(unittest.TestCase):

    selector = selector_regular.SelectorRegular()

    def test_generate(self):
        """generate one simple door with no parameter"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        custom_gate = gate.Gate(def_0)
        custom_gate.instantiation(self.selector)
        custom_gate.concrete = concrete_room.ConcreteRoom()
        custom_gate.structure.generate(custom_gate.concrete)
        custom_gate.dressing.generate(custom_gate.concrete)

        # check that it contains an object with expected parent
        objects = custom_gate.concrete.get_objects()
        self.assertEquals(len(objects), 1)
        object = objects[0]
        self.assertEquals(object.parent, "gate0")

        # check there is a portal to an outer world (sounds nice, he ?)
        phys_faces = object.get_physical_faces()
        portal_faces = [ faces for faces in phys_faces if object.PHYS_TYPE_PORTAL == faces["physics"]["type"] ]
        self.assertEquals(len(portal_faces), 1)
        portal_face = portal_faces[0]
        self.assertEquals(portal_face["physics"], {'type': 'portal', 'connect': ['room1', 'room2']})

        # won't check face itself, left to visual QC

if __name__ == '__main__':
    unittest.main()
