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

import bricks.sphere as sphere
import dressings.basic as basic

logger = logging.getLogger("TestBrickSphere")
logger.setLevel(logging.INFO)

brick_0_pad = {
            "b_id": "b1",
            "parameters": {
                "structure_class": "sphere"
            }
        }

brick_0_pad_values = {
            "b_id": "b1",
            "parameters": {
                "structure_class": "sphere"
            }
        }

class TestBrickSphere(unittest.TestCase):

    selector = selector_regular.SelectorRegular()

    def test_generate_0_pad(self) -> None:
        """generate one rectangular with no pad"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        selector = selector_regular.SelectorRegular()
        custom_brick = brick.Brick(brick_0_pad, selector)
        custom_brick.structure = sphere.BrickSphere().get_instance(custom_brick)
        custom_brick.dressing = basic.DressingBasic().get_instance(custom_brick)
        custom_brick.structure_personalization()
        custom_brick.dressing_instantiation()
        custom_brick.dressing_personalization()
        concrete = concrete_room.ConcreteRoom()
        custom_brick.finalize(concrete)
        self.assertEqual(len(concrete.objects) , 1)


if __name__ == '__main__':
    unittest.main()
