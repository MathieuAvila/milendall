"""
test rectangular room
"""

import logging
import sys
import pathlib

import unittest

import cgtypes.mat4

import dressings.basic as basic
import concrete_room

logger = logging.getLogger("TestDressingBasic")
logger.setLevel(logging.INFO)

class TestDressingBasic(unittest.TestCase):

    def test_generate_1_physics_1_visual_1_both(self):
        """ test generate 1 phys, 1 vis, 1 both"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)

        concrete = concrete_room.ConcreteRoom()
        parent = concrete.add_child(None, "parent")
        points = [
            cgtypes.vec3(0.0, 0.0, 0.0),
            cgtypes.vec3(1.0, 0.0, 0.0),
            cgtypes.vec3(0.0, 1.0, 0.0),
            cgtypes.vec3(0.0, 0.0, 1.0) ]
        index0 = parent.add_structure_points(points)
        parent.add_structure_faces(
            index0,
            [ [0,1,2] ],
            concrete_room.Node.CAT_PHYS, [concrete_room.Node.HINT_WALL], [ 0 ] )
        parent.add_structure_faces(
            index0,
            [ [1,2,3] ],
            concrete_room.Node.CAT_VIS, [concrete_room.Node.HINT_WALL], [ 0 ] )
        parent.add_structure_faces(
            index0,
            [ [2,3,0] ],
            [concrete_room.Node.CAT_PHYS_VIS], [concrete_room.Node.HINT_WALL], [ 0 ] )

        basic1 = basic.DressingBasic()

        basic1.generate(concrete)
        print(parent.dressing)

        path_gen = "/tmp/test_basic/output"
        pathlib.Path(path_gen).mkdir(parents=True, exist_ok=True)

        concrete.generate_gltf(path_gen)

        concrete_room.preview(path_gen + "/room.gltf", path_gen + "/room_preview.gltf")

if __name__ == '__main__':
    unittest.main()
