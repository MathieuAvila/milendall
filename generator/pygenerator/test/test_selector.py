"""
test level
"""

import unittest
import selector_regular
import room
from bricks import register

class TestSelector(unittest.TestCase):

    def test_get_list_room(self):
        """ test retrieving list of rooms"""

        _s = selector_regular.SelectorRegular()
        _r = room.Room("/tmp", "name", _s)
        #_s.get_from_name(name, _r)
        print(register.brickTypes)
        self.assertIsNotNone(register.brickTypes)
        self.assertTrue("rectangular" in register.brickTypes)

if __name__ == '__main__':
    unittest.main()
