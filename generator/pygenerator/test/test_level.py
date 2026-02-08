"""
test level
"""

from __future__ import annotations

import logging

import unittest
import level
import selector_fake
import jsonschema
import state
import pathlib
import shutil
import json_helper

logger = logging.getLogger("TestLevel")
logger.level = logging.DEBUG

class TestLevel(unittest.TestCase):

    def remake_dest(self, dir: str) -> None:
        pathlib.Path(dir).mkdir(parents=True, exist_ok=True)
        shutil.rmtree(dir)
        pathlib.Path(dir).mkdir(parents=True, exist_ok=True)

    def test_01_load_invalid_level(self) -> None:
        """ test loading an invalid level"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        has_error = False
        try:
            loaded_level.load("../test/test_samples/level/invalid/", state.LevelState.Instantiated)
        except jsonschema.exceptions.ValidationError:
            has_error = True
        self.assertTrue(has_error)

    def INVALID_test_01_load_save_simple(self) -> None:
        """ test loading and saving a level
        MARKED INVALID as it writes back the gates property"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        loaded_level.load("../test/test_samples/level/simple/", state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)

        self.assertIsNotNone(loaded_level.values.gates)
        self.assertEqual(len(loaded_level.values.gates), 4)

        self.assertIsNotNone(loaded_level.values.rooms)
        self.assertEqual(len(loaded_level.values.rooms), 3)

        output = "/tmp/test_01_load_save_simple"
        self.remake_dest(output)
        _dump = loaded_level.save(output)
        reloaded_level = level.Level(selector)
        reloaded_level.load(output, state.LevelState.Instantiated)
        self.assertIsNotNone(reloaded_level)

        self.assertIsNotNone(reloaded_level.values.gates)
        self.assertEqual(len(reloaded_level.values.gates), 4)

        self.assertIsNotNone(reloaded_level.values.rooms)
        self.assertEqual(len(reloaded_level.values.rooms), 3)

    def test_02_load_save_1r(self) -> None:
        """ test loading and saving a level with 1 room"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        loaded_level.load("../test/test_samples/level/simple_1r_0b_instantiated", state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)

    def test_03_structure_personalization(self) -> None:
        """Test the instantiation algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        loaded_level.load("../test/test_samples/level/simple_1r_0b_instantiated/", state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)
        loaded_level.structure_personalization()
        output = "/tmp/test_03_structure_personalization"
        self.remake_dest(output)
        loaded_level.save(output)

    def test_04_dressing_instantiation(self) -> None:
        """Test the dressing instantiation"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        loaded_level.load("../test/test_samples/level/simple_1r_0b_instantiated/", state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)
        loaded_level.structure_personalization()
        loaded_level.dressing_instantiation()
        output = "/tmp/test_04_dressing_instantiation"
        self.remake_dest(output)
        loaded_level.save(output)

    def test_05_dressing_personalization(self) -> None:
        """Test the dressng personalization algo"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        loaded_level.load("../test/test_samples/level/simple_1r_0b_instantiated/", state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)
        loaded_level.structure_personalization()
        loaded_level.dressing_instantiation()
        loaded_level.dressing_personalization()
        output = "/tmp/test_05_dressing_personalization"
        self.remake_dest(output)
        loaded_level.save(output)

    def test_06_finalize(self) -> None:
        """Test the finalize"""
        selector = selector_fake.SelectorFake()
        loaded_level = level.Level(selector)
        root_sample = "../test/test_samples/level/simple_1r_0b_instantiated/"
        loaded_level.load(root_sample, state.LevelState.Instantiated)
        self.assertIsNotNone(loaded_level)
        loaded_level.structure_personalization()
        loaded_level.dressing_instantiation()
        loaded_level.dressing_personalization()
        output = "/tmp/test_06_finalize"
        self.remake_dest(output)
        loaded_level.finalize(output, True)
        # now check validity of level file
        obj = json_helper.load_and_validate_json(
            output + "/level.json",
            "file_final_level.json")
        obj_golden_sample = json_helper.load_and_validate_json(
            root_sample + "/level.json",
            "file_final_level.json")
        self.assertIsNotNone(obj)
        self.assertIsNotNone(obj_golden_sample)
        obj["$schema"] = ""
        obj_golden_sample["$schema"] = ""
        self.assertEqual(obj, obj_golden_sample)



if __name__ == '__main__':
    unittest.main()
