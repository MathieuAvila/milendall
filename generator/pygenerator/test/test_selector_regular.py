"""test regular load"""

import unittest
import selector_regular

from rooms import register

class TestSelectorRegular(unittest.TestCase):

    def test_get_list_room_regular(self):
        """ test retrieving list of rooms"""

        _s = selector_regular.SelectorRegular()
        print(register.roomTypes)
        self.assertIsNotNone(register.roomTypes)
        self.assertTrue("rectangular" in register.roomTypes)

if __name__ == '__main__':
    unittest.main()
