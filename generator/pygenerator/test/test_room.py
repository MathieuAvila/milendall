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
import concrete_room
import cgtypes.mat4
import cgtypes.vec3
import math

logger = logging.getLogger("TestRoom")
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

    def test_02_structure_personalization(self):
        """Test the personalization algo using a fake selector"""
        my_test_dir = "/tmp/test_01_structure_personalization"
        self.remake_dest(my_test_dir)
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.save(my_test_dir)

    def test_03_dressing_instantiation(self):
        """Test the dressing instantiation algo using a fake selector"""
        my_test_dir = "/tmp/test_01_dressing_instantiation"
        self.remake_dest(my_test_dir)
        selector = selector_fake.SelectorFake()
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.save(my_test_dir)

    def test_04_dressing_personalization(self):
        """Test the dressing personalization algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        my_test_dir = "/tmp/test_01_dressing_personalization"
        self.remake_dest(my_test_dir)
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.dressing_personalization()
        loaded_room.save(my_test_dir)


    def test_05_finalize_01_2b(self):
        """Test the finalize"""
        """Test the dressing personalization algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        my_test_dir = "/tmp/test_05_finalize_01_2b"
        self.remake_dest(my_test_dir)
        loaded_room = room.Room("../test/test_samples/room/room_2b/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.dressing_personalization()
        loaded_room.save(my_test_dir)
        loaded_room.finalize(my_test_dir)

    def test_05_finalize_01_2b_linked(self):
        """Test the finalize"""
        """Test the dressing personalization algo using a fake selector"""
        selector = selector_fake.SelectorFake()
        my_test_dir = "/tmp/test_05_finalize_01_2b_linked"
        self.remake_dest(my_test_dir)
        loaded_room = room.Room("../test/test_samples/room/room_2b_linked/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.dressing_personalization()
        loaded_room.save(my_test_dir)
        loaded_room.finalize(my_test_dir)

    def test_06_finalize_brick_translation(self):
        """Test a brick root pad with translation"""
        selector = selector_fake.SelectorFake()
        my_test_dir = "/tmp/test_06_finalize_brick_translation"
        self.remake_dest(my_test_dir)
        loaded_room = room.Room("../test/test_samples/room/room_2b_root_pad_translation/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.dressing_personalization()
        loaded_room.save(my_test_dir)
        concrete = concrete_room.ConcreteRoom()
        loaded_room.finalize(my_test_dir, preview=False, concrete_test_param=concrete)
        objs = concrete.get_objects()
        obj_p = {}
        for o in objs:
            logger.info("%s => %s", o.name, o.parent)
            obj_p[o.name] = o.parent
            if o.name == "b0_pad0_root_b1":
                special = o
        self.assertEquals(obj_p , {
            "b0_parent" : None,
            "b0_pad0" : "b0_parent",
            "b0_pad0_root_b1" : "b0_pad0",
            "b1_parent":"b0_pad0_root_b1"})
        self.assertIsNotNone(special)
        self.assertEquals(special.matrix, cgtypes.mat4.translation(cgtypes.vec3(1.0,2.0,3.0)))


    def test_06_finalize_brick_rotation(self):
        """Test a brick root pad with rotation"""
        selector = selector_fake.SelectorFake()
        my_test_dir = "/tmp/test_06_finalize_brick_rotation"
        self.remake_dest(my_test_dir)
        loaded_room = room.Room("../test/test_samples/room/room_2b_root_pad_rotation/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.dressing_personalization()
        loaded_room.save(my_test_dir)
        concrete = concrete_room.ConcreteRoom()
        loaded_room.finalize(my_test_dir, preview=False, concrete_test_param=concrete)
        objs = concrete.get_objects()
        obj_p = {}
        for o in objs:
            logger.info("%s => %s", o.name, o.parent)
            obj_p[o.name] = o.parent
            if o.name == "b0_pad0_root_b1":
                special = o
        self.assertEquals(obj_p , {
            "b0_parent" : None,
            "b0_pad0" : "b0_parent",
            "b0_pad0_root_b1" : "b0_pad0",
            "b1_parent":"b0_pad0_root_b1"})
        self.assertIsNotNone(special)
        diff = special.matrix - cgtypes.mat4.rotation(math.pi, cgtypes.vec3(0.0,1.0,0.0))
        sum = 0.0
        for i in range(0,4):
            for j in range(0,4):
               sum = sum + abs(diff[i][j])
        self.assertTrue(sum < 0.1)


    def test_06_finalize_brick_rotation_translation(self):
        """Test a brick root pad with rotation translation"""
        selector = selector_fake.SelectorFake()
        my_test_dir = "/tmp/test_06_finalize_brick_rotation_translation"
        self.remake_dest(my_test_dir)
        loaded_room = room.Room("../test/test_samples/room/room_2b_root_pad_rotation_translation/", "room1", selector)
        self.assertIsNotNone(loaded_room)
        loaded_room.load(state.LevelState.Instantiated)
        loaded_room.structure_personalization()
        loaded_room.dressing_instantiation()
        loaded_room.dressing_personalization()
        loaded_room.save(my_test_dir)
        concrete = concrete_room.ConcreteRoom()
        loaded_room.finalize(my_test_dir, preview=False, concrete_test_param=concrete)
        objs = concrete.get_objects()
        obj_p = {}
        for o in objs:
            logger.info("%s => %s", o.name, o.parent)
            obj_p[o.name] = o.parent
            if o.name == "b0_pad0_root_b1":
                special = o
        self.assertEquals(obj_p , {
            "b0_parent" : None,
            "b0_pad0" : "b0_parent",
            "b0_pad0_root_b1" : "b0_pad0",
            "b1_parent":"b0_pad0_root_b1"})
        self.assertIsNotNone(special)
        diff = special.matrix - (
                    cgtypes.mat4.translation(cgtypes.vec3(1.0,2.0,3.0)) *
                    cgtypes.mat4.rotation(math.pi, cgtypes.vec3(0.0,1.0,0.0))
                    )

        sum = 0.0
        for i in range(0,4):
            for j in range(0,4):
               sum = sum + abs(diff[i][j])
        self.assertTrue(sum < 0.1)


if __name__ == '__main__':
    unittest.main()
