"""
test level
"""

import logging
import json
import pathlib
import struct
import sys

import unittest
import concrete_room
import animation
import cgtypes.mat4

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)


class TestAnimation(unittest.TestCase):

    def get_data_from_accessor(self, json_content, index, root_path):
        """return accessor data in table, format depends on type"""
        accessor = json_content["accessors"][index]
        data_type = accessor["type"]
        data_count = accessor["count"]
        data_low_type = accessor["componentType"]
        buffer_view = json_content["bufferViews"][accessor["bufferView"]]
        byte_length = buffer_view["byteLength"]
        buffer = json_content["buffers"][buffer_view["buffer"]]
        file_path = root_path + "/" + buffer["uri"]
        offset = buffer_view["byteOffset"]
        data_file = open(file_path, "rb")
        data_file.seek(offset)
        if data_type == "VEC3":
            self.assertEqual(data_low_type, 5126)  # expect a float
            self.assertEqual(byte_length, data_count * 3 * 4) # 4 bytes for each float, and 3 to make a vector
            result = []
            for i in range(data_count):
                read_p = struct.unpack('fff', data_file.read(4*3))
                result.append(cgtypes.vec3(read_p[0],read_p[1],read_p[2]))
            return result
        if data_type == "VEC2":
            self.assertEqual(data_low_type, 5126)  # expect a float
            self.assertEqual(byte_length, data_count * 3 * 4) # 4 bytes for each float, and 2 to make a vector
            result = []
            for i in range(data_count):
                read_p = struct.unpack('ff', data_file.read(4*2))
                result.append(cgtypes.vec3(read_p[0],read_p[1]))
            return result
        if data_type == "SCALAR":
            self.assertEqual(data_low_type, 5123)  # expect a short int
            self.assertEqual(byte_length, data_count * 2) # 2 bytes for each
            result = []
            for i in range(data_count):
                read_p = struct.unpack('H', data_file.read(2))
                result.append(read_p[0])
            return result

    def create_cube(self):
        """ create a cube for future use """
        room = concrete_room.ConcreteRoom()
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        parent.add_dressing_faces(
            [
            cgtypes.vec3(0,0,0),
            cgtypes.vec3(0,0,1),
            cgtypes.vec3(0,1,1),
            cgtypes.vec3(0,1,0),

            cgtypes.vec3(1,0,0),
            cgtypes.vec3(1,0,1),
            cgtypes.vec3(1,1,1),
            cgtypes.vec3(1,1,0),
            ],
            [
                # X,Y
                [5,6,2,1],
                [0,3,7,4],
             ],
            concrete_room.get_texture_definition(
                "common/basic/wall.jpg",
                axes=[ ["x",],["y"] ],
                scale=1.0 ))

        parent.add_dressing_faces(
            [
            cgtypes.vec3(0,0,0),
            cgtypes.vec3(0,0,1),
            cgtypes.vec3(0,1,1),
            cgtypes.vec3(0,1,0),

            cgtypes.vec3(1,0,0),
            cgtypes.vec3(1,0,1),
            cgtypes.vec3(1,1,1),
            cgtypes.vec3(1,1,0),
            ],
            [
                # Y,Z
                [0,1,2,3],
                [7,6,5,4],

                # X,Z
                [4,5,1,0],
                [6,7,3,2],
             ],
            concrete_room.get_texture_definition(
                "common/basic/ground.jpg",
                axes=[ ["x", "y"],["z"] ],
                scale=1.0 ))
        return room

    def test_insert_1_animation(self):
        """Test generation of 1 animation in GLTF file"""
        cube = self.create_cube()
        anim0 = animation.Animation("test_anim", 0.0, 1.0, "my_event")
        anim0.append_action("parent", anim0.ACTION_TRANSLATION, [
            { "time":1.0, "value": [0.0, 0.0 ,0.0] },
            { "time":2.0, "value": [3.0, 0.0 ,0.0] },
            ])
        cube.add_animation(anim0)

        path_gen = "/tmp/test_animation/output"
        pathlib.Path(path_gen).mkdir(parents=True, exist_ok=True)
        try:
            cube.generate_gltf(path_gen)
            concrete_room.preview(path_gen + "/room.gltf", path_gen + "/room_preview.gltf")
        except:
            print("Unexpected error:", sys.exc_info())







if __name__ == '__main__':
    unittest.main()
