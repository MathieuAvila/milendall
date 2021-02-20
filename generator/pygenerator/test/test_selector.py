"""
test level
"""

import unittest
import selector_regular
import room
from rooms import register

class TestSelector(unittest.TestCase):

    def test_get_list_room(self):
        """ test retrieving list of rooms"""

        _s = selector_regular.SelectorRegular()
        _r = room.Room({})
        #_s.get_from_name(name, _r)
        print(register.roomTypes)
        self.assertIsNotNone(register.roomTypes)
        self.assertTrue("rectangular" in register.roomTypes)

if __name__ == '__main__':
    unittest.main()
