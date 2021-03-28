"""
test rectangular room
"""

import logging
import sys

import unittest
import rooms.rectangular as rectangular
import room
import concrete_room

logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)

logger = logging.getLogger()
logger.level = logging.DEBUG

def_0 = {
   "room_id": "room1",
   "structure_class": "rectangular",
  }

class TestRoomRectangular(unittest.TestCase):

    def test_generate_0_gate(self):
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        rect0 = rectangular.RectangularRoom()
        self.assertIsNotNone(rect0)
        custom_room = room.Room(def_0)
        rect1 = rect0.get_instance(custom_room)
        self.assertIsNotNone(rect1)
        rect1.instantiate_defaults()
        rect1.instantiate()
        concrete = concrete_room.ConcreteRoom()
        rect1.generate(concrete)


if __name__ == '__main__':
    unittest.main()
