"""
test level
"""

import unittest
import level

class TestLevel(unittest.TestCase):

    def test_load_save(self):
        """ test loading and saving a level"""
        loaded_level = None
        self.assertEqual('foo'.upper(), 'FOO')
        loaded_level = level.Level("../../samples/simple/level-user.json")
        self.assertIsNotNone(loaded_level)

        self.assertIsNotNone(loaded_level.values.gates)
        self.assertEqual(len(loaded_level.values.gates), 4)

        self.assertIsNotNone(loaded_level.values.rooms)
        self.assertEqual(len(loaded_level.values.rooms), 3)

        _dump = loaded_level.save("/tmp/level.json")
        reloaded_level = level.Level("/tmp/level.json")
        self.assertIsNotNone(reloaded_level)

        self.assertIsNotNone(reloaded_level.values.gates)
        self.assertEqual(len(reloaded_level.values.gates), 4)

        self.assertIsNotNone(reloaded_level.values.rooms)
        self.assertEqual(len(reloaded_level.values.rooms), 3)


if __name__ == '__main__':
    unittest.main()
