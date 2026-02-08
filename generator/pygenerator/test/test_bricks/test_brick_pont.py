"""
test pont (bridge) brick
"""

from __future__ import annotations

import logging
import sys
import unittest

import brick
import concrete_room
import selector_regular

import bricks.pont as pont
import dressings.basic as basic

logger = logging.getLogger("TestBrickPont")
logger.setLevel(logging.INFO)


brick_0 = {
    "b_id": "bp1",
    "parameters": {
        "structure_class": "pont"
    }
}

brick_1_values = {
    "b_id": "bp2",
    "parameters": {
        "structure_class": "pont",
        "structure_parameters": {
            "size": [8.0, 0.8, 4.0],
            "rail_height": 1.5,
            "reverse": False,
        }
    }
}


class TestBrickPont(unittest.TestCase):

    selector = selector_regular.SelectorRegular()

    def test_generate_default(self) -> None:
        """generate one pont with default parameters and ensure faces exist"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        selector = selector_regular.SelectorRegular()
        custom_brick = brick.Brick(brick_0, selector)
        custom_brick.structure = pont.BrickPont().get_instance(custom_brick)
        custom_brick.dressing = basic.DressingBasic().get_instance(custom_brick)
        custom_brick.structure_personalization()
        logger.info(custom_brick.values.parameters)
        self.assertEqual(
            custom_brick.values.parameters,
            {
                'structure_class': 'pont',
                'structure_private': {'size': [6.0, 0.5, 3.0], 'rail_height': 1.2, 'reverse': False},
                'structure_parameters': {}
            }
        )
        custom_brick.dressing_instantiation()
        custom_brick.dressing_personalization()
        concrete = concrete_room.ConcreteRoom()
        custom_brick.finalize(concrete)
        # One object added
        self.assertEqual(len(concrete.objects), 1)
        # Check physical faces count equals 4 (deck top, deck bottom, two rails)
        parent = concrete.get_node("parent")
        self.assertIsNotNone(parent)
        phys = parent.get_physical_faces()
        self.assertEqual(len(phys), 4)

    def test_generate_values(self) -> None:
        """generate one pont with specific parameters and ensure faces exist"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        selector = selector_regular.SelectorRegular()
        custom_brick = brick.Brick(brick_1_values, selector)
        custom_brick.structure = pont.BrickPont().get_instance(custom_brick)
        custom_brick.dressing = basic.DressingBasic().get_instance(custom_brick)
        custom_brick.structure_personalization()
        logger.info(custom_brick.values.parameters)
        self.assertEqual(
            custom_brick.values.parameters.structure_private,
            {'size': [8.0, 0.8, 4.0], 'rail_height': 1.5, 'reverse': False}
        )
        custom_brick.dressing_instantiation()
        custom_brick.dressing_personalization()
        concrete = concrete_room.ConcreteRoom()
        custom_brick.finalize(concrete)
        self.assertEqual(len(concrete.objects), 1)
        parent = concrete.get_node("parent")
        self.assertIsNotNone(parent)
        # Ensure 4 physical faces are present
        self.assertEqual(len(parent.get_physical_faces()), 4)


if __name__ == '__main__':
    unittest.main()
