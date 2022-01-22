"""
test rectangular room
"""

import logging
import sys

import unittest
import bricks.rectangular as rectangular
import gates.simple_door as simple_door
import brick
import gate
import concrete_room
import selector_regular

import dressings.basic as basic

logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)

logger = logging.getLogger()
logger.level = logging.DEBUG

brick_0_pad = {
            "b_id": "b1",
            "parameters": {
                "structure_class": "rectangular"
            }
        }

class TestBrickRectangular(unittest.TestCase):

    selector = selector_regular.SelectorRegular()

    def test_generate_0_pad(self):
        """generate one rectangular with no pad"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        selector = selector_regular.SelectorRegular()
        custom_brick = brick.Brick(brick_0_pad, selector)
        custom_brick.structure = rectangular.BrickRectangular().get_instance(custom_brick)
        custom_brick.dressing = basic.DressingBasic().get_instance(custom_brick)
        custom_brick.structure_personalization()
        custom_brick.dressing_instantiation()
        custom_brick.dressing_personalization()
        concrete = concrete_room.ConcreteRoom()
        custom_brick.finalize(concrete)
        self.assertEqual(len(concrete.objects) , 1)


if __name__ == '__main__':
    unittest.main()
