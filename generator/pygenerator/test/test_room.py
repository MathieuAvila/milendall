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
        loaded_room = room.Room("../test/test_samples/room/room_0b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)

        self.assertIsNotNone(loaded_room.values.bricks)
        self.assertEqual(len(loaded_room.values.bricks), 0)

        # todo: check connections

        output = "/tmp/test_01_load_save_void"
        self.remake_dest(output)
        _dump = loaded_room.save(output)
        reloaded_room = room.Room(output, "room1", selector)
        self.assertIsNotNone(reloaded_room)

        logger.info(reloaded_room.values)

        self.assertIsNotNone(reloaded_room.values.bricks)
        self.assertEqual(len(reloaded_room.values.bricks), 0)

    def test_01_load_save_2b(self):
        """ test loading and saving a void room"""
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)

        self.assertIsNotNone(loaded_room.values.bricks)
        self.assertEqual(len(loaded_room.values.bricks), 2)

        # todo: check connections

        output = "/tmp/test_01_load_save_2b"
        self.remake_dest(output)
        _dump = loaded_room.save(output)
        reloaded_room = room.Room(output, "room1", selector)
        reloaded_room.load(state.LevelState.Instantiated)
        self.assertIsNotNone(reloaded_room)

        logger.info(reloaded_room.values)

        self.assertIsNotNone(reloaded_room.values.bricks)
        self.assertEqual(len(reloaded_room.values.bricks), 2)

    def test_01_structure_personalization(self):
        """Test the personalization algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.save("/tmp/test_01_structure_personalization")

    def test_01_dressing_instantiation(self):
        """Test the dressing instantiation algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.save("/tmp/test_01_dressing_instantiation")

    def test_01_dressing_finalization(self):
        """Test the dressing personalization algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.dressing_personalization()
        loaded_room.save("/tmp/test_01_dressing_finalization")


    def test_finalize(self):
        """Test the finalize"""
        # todo: for real


if __name__ == '__main__':
    unittest.main()
