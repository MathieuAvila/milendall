"""
test brick
"""

from __future__ import annotations

import logging

import unittest
import brick
import selector_regular

logger = logging.getLogger("TestRoom")
logger.setLevel(logging.INFO)

def_scheme_invalid_pad = {
   "b_id": "b0",
   "pads": [
       {
            "pad_id" : "p1",
            "definition" : {
               "INVALID" : [ 1.0, 2.0, 3.0]
           }
       }
    ],
   "parameters": {
       "structure_class": "simple_pad_provider"
   }
  }

def_scheme_invalid_parameters = {
   "b_id": "b0",
   "parameters": {
       "structure_class": "rectangular",
       "structure_parameters": {
           "INVALID": 0
       }
   }
  }

def_scheme_invalid_structure_class = {
   "b_id": "b0",
   "parameters": {
       "structure_class": "INVALID"
   }
  }

def_scheme_invalid_dressing_class = {
   "b_id": "b0",
   "parameters": {
       "structure_class": "rectangular",
       "dressing_class": "INVALID"
   }
  }


class TestBrick(unittest.TestCase):

    def test_01_load_invalid_pad(self) -> None:
        """ test loading and saving a brick with a PAD incorrectly defined"""
        selector = selector_regular.SelectorRegular()
        with self.assertRaises(Exception):
            brick.Brick(def_scheme_invalid_pad, selector)

    def test_01_load_invalid_parameters(self) -> None:
        """ test loading and saving a brick with incorrect parameters"""
        selector = selector_regular.SelectorRegular()
        with self.assertRaises(Exception):
            brick.Brick(def_scheme_invalid_parameters, selector)

    def test_01_load_invalid_structure_class(self) -> None:
        """ test loading and saving a brick with incorrect structure class"""
        selector = selector_regular.SelectorRegular()
        with self.assertRaises(Exception):
            b = brick.Brick(def_scheme_invalid_structure_class, selector)
            b.structure_personalization()

    def test_01_load_invalid_dressing_class(self) -> None:
        """ test loading and saving a brick with incorrect dressing class"""
        selector = selector_regular.SelectorRegular()
        with self.assertRaises(Exception):
            b = brick.Brick(def_scheme_invalid_dressing_class, selector)
            b.dressing_personalization()


if __name__ == '__main__':
    unittest.main()
