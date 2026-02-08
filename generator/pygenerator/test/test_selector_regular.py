"""test regular load"""

from __future__ import annotations

import unittest

import selector_regular

from bricks import register as room_register

class TestSelectorRegular(unittest.TestCase):

    def test_get_list_bricks_regular(self) -> None:
        """ test retrieving list of bricks"""

        _s = selector_regular.SelectorRegular()
        print(room_register.brickTypes)
        self.assertIsNotNone(room_register.brickTypes)
        self.assertTrue("rectangular" in room_register.brickTypes)


if __name__ == '__main__':
    unittest.main()
