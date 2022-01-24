"""
test rectangular room
"""

import sys
import logging
import unittest
import bricks.rectangular as rectangular
import brick
import concrete_room
import selector_regular

import dressings.basic as basic

logger = logging.getLogger("TestBrickRectangular")
logger.setLevel(logging.INFO)


brick_0_pad = {
            "b_id": "b1",
            "parameters": {
                "structure_class": "rectangular"
            }
        }

brick_1_pad_values = {
            "b_id": "b1",
            "parameters": {
                "structure_class": "rectangular",
                "structure_parameters": {
                    "size" : [1.0,2.0,3.0]
                }
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
        logger.info(custom_brick.values.parameters)
        self.assertEqual(custom_brick.values.parameters ,
                         {
                            'structure_class': 'rectangular',
                            'structure_private': {'size': [5.0, 2.5, 5.0]},
                            'structure_parameters': {}
                        })
        custom_brick.dressing_instantiation()
        custom_brick.dressing_personalization()
        concrete = concrete_room.ConcreteRoom()
        custom_brick.finalize(concrete)
        self.assertEqual(len(concrete.objects) , 1)


    def test_generate_0_pad_values(self):
        """generate one rectangular with no pad"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        selector = selector_regular.SelectorRegular()
        custom_brick = brick.Brick(brick_1_pad_values, selector)
        custom_brick.structure = rectangular.BrickRectangular().get_instance(custom_brick)
        custom_brick.dressing = basic.DressingBasic().get_instance(custom_brick)
        custom_brick.structure_personalization()
        logger.info(custom_brick.values.parameters)
        self.assertEqual(custom_brick.values.parameters.structure_private ,
                         {'size': [1.0, 2.0, 3.0]})
        custom_brick.dressing_instantiation()
        custom_brick.dressing_personalization()
        concrete = concrete_room.ConcreteRoom()
        custom_brick.finalize(concrete)
        self.assertEqual(len(concrete.objects) , 1)


if __name__ == '__main__':
    unittest.main()
