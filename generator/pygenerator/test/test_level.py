"""
test level
"""

import logging

import unittest
import level
import selector_fake
import jsonschema

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

class TestLevel(unittest.TestCase):

    def test_invalid_level(self):
        """ test loading an invalid level"""
        selector = selector_fake.SelectorFake()
        has_error = False
        self.assertEqual('foo'.upper(), 'FOO')
        try:
            level.Level("../test/test_samples/invalid/level-user.json", selector)
        except jsonschema.exceptions.ValidationError:
            has_error = True
        self.assertTrue(has_error)

    def test_load_save(self):
        """ test loading and saving a level"""
        selector = selector_fake.SelectorFake()
        loaded_level = None
        self.assertEqual('foo'.upper(), 'FOO')
        loaded_level = level.Level("../test/test_samples/simple/level-user.json", selector)
        self.assertIsNotNone(loaded_level)

        self.assertIsNotNone(loaded_level.values.gates)
        self.assertEqual(len(loaded_level.values.gates), 4)

        self.assertIsNotNone(loaded_level.values.rooms)
        self.assertEqual(len(loaded_level.values.rooms), 3)

        # todo: check connections

        _dump = loaded_level.save("/tmp/level.json")
        reloaded_level = level.Level("/tmp/level.json", selector)
        self.assertIsNotNone(reloaded_level)

        self.assertIsNotNone(reloaded_level.values.gates)
        self.assertEqual(len(reloaded_level.values.gates), 4)

        self.assertIsNotNone(reloaded_level.values.rooms)
        self.assertEqual(len(reloaded_level.values.rooms), 3)

    def test_instantiate(self):
        """Test the instantiation algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level("../test/test_samples/simple/level-user.json", selector)
        self.assertIsNotNone(loaded_level)
        loaded_level.instantiation()
        loaded_level.save("../test/test_samples/simple/level-user-instantiated.json")

    def test_dressing(self):
        """Test the instantiation algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level("../test/test_samples/simple/level-user.json", selector)
        self.assertIsNotNone(loaded_level)
        loaded_level.instantiation()
        loaded_level.dressing("/tmp/test_dressing")

    def test_finalize(self):
        """Test the finalize"""
        # todo: for real


if __name__ == '__main__':
    unittest.main()
