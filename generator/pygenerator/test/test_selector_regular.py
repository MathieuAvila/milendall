"""test regular load"""

import unittest
import selector_regular

from bricks import register as room_register
from gates import register as gate_register

class TestSelectorRegular(unittest.TestCase):

    def test_get_list_room_regular(self):
        """ test retrieving list of rooms"""

        _s = selector_regular.SelectorRegular()
        print(room_register.brickTypes)
        self.assertIsNotNone(room_register.brickTypes)
        self.assertTrue("rectangular" in room_register.brickTypes)

    def test_get_list_gate_regular(self):
        """ test retrieving list of gates"""

        _s = selector_regular.SelectorRegular()
        print(gate_register.gateTypes)
        self.assertIsNotNone(room_register.brickTypes)
        self.assertTrue("rectangular" in room_register.brickTypes)

if __name__ == '__main__':
    unittest.main()
