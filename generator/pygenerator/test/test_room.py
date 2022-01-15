"""
test room
"""

import logging

import unittest
import room
import selector_fake
import state
import pathlib
import shutil

logging.basicConfig()
logger = logging.getLogger()
logger.setLevel(logging.INFO)

class TestRoom(unittest.TestCase):

    def remake_dest(self, dir):
        pathlib.Path(dir).mkdir(parents=True, exist_ok=True)
        shutil.rmtree(dir)
        pathlib.Path(dir).mkdir(parents=True, exist_ok=True)

    def test_01_load_save_void(self):
        """ test loading and saving a void room"""
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_0b/", "room1")
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)

        self.assertIsNotNone(loaded_room.values.bricks)
        self.assertEqual(len(loaded_room.values.bricks), 0)

        # todo: check connections

        output = "/tmp/test_01_load_save_void"
        self.remake_dest(output)
        _dump = loaded_room.save(output)
        reloaded_room = room.Room(output, selector)
        self.assertIsNotNone(reloaded_room)

        logger.info(reloaded_room.values)

        self.assertIsNotNone(reloaded_room.values.bricks)
        self.assertEqual(len(reloaded_room.values.bricks), 0)

    def test_01_load_save_2b(self):
        """ test loading and saving a void room"""
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1")
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)

        self.assertIsNotNone(loaded_room.values.bricks)
        self.assertEqual(len(loaded_room.values.bricks), 2)

        # todo: check connections

        output = "/tmp/test_01_load_save_2b"
        self.remake_dest(output)
        _dump = loaded_room.save(output)
        reloaded_room = room.Room(output, "room1")
        reloaded_room.load(state.LevelState.Instantiated)
        self.assertIsNotNone(reloaded_room)

        logger.info(reloaded_room.values)

        self.assertIsNotNone(reloaded_room.values.bricks)
        self.assertEqual(len(reloaded_room.values.bricks), 2)

    def test_01_personalize(self):
        """Test the personalization algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1")
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.personalization(selector)
        loaded_room.save("/tmp/test_01_personalize")

    def test_dressing(self):
        """Test the instantiation algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/simple_instantiated/rooms-logic.json", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.instantiation()
        loaded_room.finalize("/tmp/test_dressing")

    def test_finalize(self):
        """Test the finalize"""
        # todo: for real


if __name__ == '__main__':
    unittest.main()
