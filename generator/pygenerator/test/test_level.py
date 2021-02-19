"""
test level
"""

import unittest
import level

class TestLevel(unittest.TestCase):

    def test_load(self):
        """ test loading a level"""
        loaded_level = None
        self.assertEqual('foo'.upper(), 'FOO')
        loaded_level = level.Level("../../samples/simple/level-user.json")
        self.assertIsNotNone(loaded_level)

if __name__ == '__main__':
    unittest.main()
