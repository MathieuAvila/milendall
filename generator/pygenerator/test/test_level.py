"""
test level
"""

import logging

import unittest
import level
import selector_fake
import jsonschema
import state
import pathlib
import shutil

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

class TestLevel(unittest.TestCase):

    def remake_dest(self, dir):
        pathlib.Path(dir).mkdir(parents=True, exist_ok=True)
        shutil.rmtree(dir)
        pathlib.Path(dir).mkdir(parents=True, exist_ok=True)

    def test_invalid_level(self):
        """ test loading an invalid level"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        has_error = False
        try:
            loaded_level.load("../test/test_samples/level/invalid/", state.LevelState.Instantiated)
        except jsonschema.exceptions.ValidationError:
            has_error = True
        self.assertTrue(has_error)

    def test_01_load_save_simple(self):
        """ test loading and saving a level"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        loaded_level.load("../test/test_samples/level/simple/", state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)

        self.assertIsNotNone(loaded_level.values.gates)
        self.assertEqual(len(loaded_level.values.gates), 4)

        self.assertIsNotNone(loaded_level.values.rooms)
        self.assertEqual(len(loaded_level.values.rooms), 3)

        output = "/tmp/test_level_load"
        self.remake_dest(output)
        _dump = loaded_level.save(output)
        reloaded_level = level.Level(selector)
        reloaded_level.load(output, state.LevelState.Instantiated)
        self.assertIsNotNone(reloaded_level)

        self.assertIsNotNone(reloaded_level.values.gates)
        self.assertEqual(len(reloaded_level.values.gates), 4)

        self.assertIsNotNone(reloaded_level.values.rooms)
        self.assertEqual(len(reloaded_level.values.rooms), 3)

    def test_02_load_save_1r(self):
        """ test loading and saving a level with 1 room"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        loaded_level.load("../test/test_samples/level/simple_1r_instantiated", state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)

    def test_instantiate(self):
        """Test the instantiation algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        loaded_level.load("../test/test_samples/level/simple_1r_instantiated/", state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)
        loaded_level.personalization()
        output = "/tmp/simple_1r_instantiated"
        self.remake_dest(output)
        loaded_level.save(output)


if __name__ == '__main__':
    unittest.main()
