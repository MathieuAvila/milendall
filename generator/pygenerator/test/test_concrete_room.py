"""
test level
"""

from __future__ import annotations

import logging
import json
import pathlib
import struct
from typing import Any

import unittest
import concrete_room
import gltf_helper
import cgtypes.mat4

logger = logging.getLogger("TestConcreteRoomImpl")
logger.setLevel(logging.INFO)

from gltf_helper import get_texture_definition_with_function

def my_helper_mapper(points: list[object], face: list[int], context: dict[str, object],
                     my_points: list[object]) -> None:
    # need to fill my_points
    for p in my_points:
        p.u = p.x + 1.0
        p.v = p.y + 1.0

class TestConcreteRoomImpl(unittest.TestCase):



    def get_data_from_accessor(self, json_content: dict[str, Any], index: int,
                               root_path: str) -> list[Any]:
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


    def test_parent_child(self) -> None:
        """Test creating a simple impl with parent/child relationships for nodes"""
        room = concrete_room.ConcreteRoom()
        mat_parent2 = cgtypes.mat4(2.0)
        parent1 = room.add_child(None, "parent1")
        parent2 = room.add_child(None, "parent2" , mat_parent2)
        child2_1 = room.add_child("parent2", "child2_1")
        child2_2 = room.add_child("parent2", "child2_2")
        self.assertIsNotNone(parent1)
        self.assertIsNotNone(parent2)
        self.assertIsNotNone(child2_1)
        self.assertIsNotNone(child2_2)

        j = room.dump_to_json()

    def test_structure_faces(self) -> None:
        """Test creating a simple impl with 1 node with multiple points and faces
        sets for structure"""
        room = concrete_room.ConcreteRoom()
        self.assertIsNotNone(room)
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        index0 = parent.add_structure_points([ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ])
        self.assertEqual(0, index0)
        index1 = parent.add_structure_points([ cgtypes.vec3(3), cgtypes.vec3(4), cgtypes.vec3(5) ])
        self.assertEqual(3, index1)
        parent.add_structure_faces(
            index1,
            [ [0,1,2], [3,4,5], [6,7,8] ],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], [ 0 ] )
        parent.add_structure_faces(
            index1,
            [ [9,10,11], [12,13,14]],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_CEILING], [ 1 ] )
        j = json.loads(room.dump_to_json())
        points = j["objects"][0]["structure_points"]
        self.assertEqual(points,
        [{'x': 0.0, 'y': 0.0, 'z': 0.0},
        {'x': 1.0, 'y': 1.0, 'z': 1.0},
        {'x': 2.0, 'y': 2.0, 'z': 2.0},
        {'x': 3.0, 'y': 3.0, 'z': 3.0},
        {'x': 4.0, 'y': 4.0, 'z': 4.0},
        {'x': 5.0, 'y': 5.0, 'z': 5.0}])
        faces = j["objects"][0]["structure_faces"]
        self.assertEqual(faces,
            [
                {
                    'faces': [[3, 4, 5], [6, 7, 8], [9, 10, 11]],
                    'category': ['phy'],
                    'hints': ['building'],
                    'physics': [0]
                },
                {
                    'faces': [[12, 13, 14], [15, 16, 17]],
                    'category': ['phy'],
                    'hints': ['ceiling'],
                    'physics': [1]
                }
            ])

    def test_structure_faces_dump(self) -> None:
        """Test generation of structural faces in gltf file"""
        room = concrete_room.ConcreteRoom()
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        index0 = parent.add_structure_points([
            cgtypes.vec3(0),
            cgtypes.vec3(1),
            cgtypes.vec3(2),
            cgtypes.vec3(3),
            cgtypes.vec3(4),
            cgtypes.vec3(5),
            cgtypes.vec3(6),
            cgtypes.vec3(7),
            ])
        self.assertEqual(0, index0)
        data_0 = { "type":"gate", "gate_target":"roomX"} # intended to be added as is
        parent.add_structure_faces(
            index0,
            [ [0,1,2,3], [4,5,6,7] ],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], data_0 )
        data_1 = { "any_other_metadata" : 0 } # intended to be added as is
        parent.add_structure_faces(
            index0,
            [ [0,2,4]],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING],
                data_1 )
        path_gen = "/tmp/test_dump_structure_faces"
        pathlib.Path(path_gen).mkdir(parents=True, exist_ok=True)
        room.generate_gltf(path_gen)
        with open(path_gen + "/room.gltf", "r") as room_file:
            obj = json.load(room_file)
        parent_object = obj["nodes"][1]
        self.assertEqual(parent_object, {
            'name': 'parent',
            'extras': {
                'phys_faces': [
                    {
                    'data': data_0,
                    'accessor': 1 # expect in call order, although not in contract
                    }, {
                    'data':data_1,
                    'accessor': 2 # expect in call order, although not in contract
                    }],
                'points': 0 # 'points first' not in contract
                }
            }
        )
        # check points
        data_points = self.get_data_from_accessor(obj, 0, path_gen)
        self.assertEqual(len(data_points), 8)
        self.assertEqual(data_points[0], cgtypes.vec3(0))
        self.assertEqual(data_points[1], cgtypes.vec3(1))
        self.assertEqual(data_points[2], cgtypes.vec3(2))
        self.assertEqual(data_points[3], cgtypes.vec3(3))
        self.assertEqual(data_points[4], cgtypes.vec3(4))
        self.assertEqual(data_points[5], cgtypes.vec3(5))
        self.assertEqual(data_points[6], cgtypes.vec3(6))
        self.assertEqual(data_points[7], cgtypes.vec3(7))
        # check 1st row of indexes
        data_points = self.get_data_from_accessor(obj, 1, path_gen)
        self.assertEqual(len(data_points), 10)
        self.assertEqual(data_points, [4, 0, 1, 2, 3, 4, 4, 5, 6, 7])
        # check 2nd row of indexes
        data_points = self.get_data_from_accessor(obj, 2, path_gen)
        self.assertEqual(len(data_points), 4)
        self.assertEqual(data_points, [3, 0, 2, 4])



    def test_get_visual_faces(self) -> None:
        """test that we can get visuals only"""
        room = concrete_room.ConcreteRoom()
        parent = room.add_child(None, "parent")
        table_points = [ cgtypes.vec3(0),
                cgtypes.vec3(1),
                cgtypes.vec3(2),
                cgtypes.vec3(3),
                cgtypes.vec3(4),
                cgtypes.vec3(5),
                cgtypes.vec3(6),
                cgtypes.vec3(7),
                ]
        index0 = parent.add_structure_points(table_points)

        list_phys_building = [ [0,1,2], [0,1,3] ]
        parent.add_structure_faces(
            index0,
            list_phys_building,
            concrete_room.Node.CAT_PHYS, [concrete_room.Node.HINT_BUILDING], [ 0 ] )

        list_vis_ceiling = [ [1,2,3], [1,3,4]]
        parent.add_structure_faces(
            index0,
            list_vis_ceiling,
            concrete_room.Node.CAT_VIS, [concrete_room.Node.HINT_CEILING], [ 1 ] )

        list_phys_vis_building = [ [2,3,4], [2,4,5]]
        parent.add_structure_faces(
            index0,
            list_phys_vis_building,
            concrete_room.Node.CAT_PHYS_VIS, [concrete_room.Node.HINT_BUILDING], [ 1 ] )

        list_phys_vis_ceiling = [ [2,3,4], [2,4,5]]
        parent.add_structure_faces(
            index0,
            list_phys_vis_ceiling,
            concrete_room.Node.CAT_PHYS_VIS, [concrete_room.Node.HINT_CEILING], [ 1 ] )

        # check it returns 1 element that matches
        list_building = parent.get_visual_face([concrete_room.Node.HINT_BUILDING])
        self.assertEqual(1, len(list_building))
        self.assertListEqual(list_building[0]["faces"], list_phys_vis_building)

        # check it returns 2 elements that match
        list_ceiling = parent.get_visual_face([concrete_room.Node.HINT_CEILING])
        self.assertEqual(2, len(list_ceiling))
        self.assertListEqual(list_ceiling[0]["faces"], list_vis_ceiling)
        self.assertListEqual(list_ceiling[1]["faces"], list_phys_vis_ceiling)

        # check it returns 0 elements
        list_building_ceiling = parent.get_visual_face(
            [concrete_room.Node.HINT_BUILDING,
            concrete_room.Node.HINT_CEILING])
        self.assertEqual(0, len(list_building_ceiling))

    def test_dressing_filter_points(self) -> None:
        """test filtering points when adding faces, so that only useful points are kept
        and there is no useless data"""
        room = concrete_room.ConcreteRoom()
        self.assertIsNotNone(room)
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        parent.add_dressing_faces(
            [
                cgtypes.vec3(0),
                cgtypes.vec3(1),
                cgtypes.vec3(1),
                cgtypes.vec3(2),
                cgtypes.vec3(3),
                cgtypes.vec3(4),
                cgtypes.vec3(5),
                cgtypes.vec3(6),
                cgtypes.vec3(7),
                cgtypes.vec3(8),
                cgtypes.vec3(9),
                cgtypes.vec3(10)
            ],
            [ [1,2,3], [2,3,4], [6,7,8]],
            concrete_room.get_texture_definition("myfilename"))
        j = json.loads(room.dump_to_json())
        faces = j["objects"][0]["dressing"]
        self.assertEqual(faces, {
            'myfilename': {
                'points': [
                    {'x': 1.0, 'y': 1.0, 'z': 1.0, 'u': 1.0, 'v': 1.0},
                    {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 2.0, 'v': 2.0},
                    {'x': 3.0, 'y': 3.0, 'z': 3.0, 'u': 3.0, 'v': 3.0},
                    {'x': 5.0, 'y': 5.0, 'z': 5.0, 'u': 5.0, 'v': 5.0},
                    {'x': 6.0, 'y': 6.0, 'z': 6.0, 'u': 6.0, 'v': 6.0},
                    {'x': 7.0, 'y': 7.0, 'z': 7.0, 'u': 7.0, 'v': 7.0}],
                'faces': [
                    [0, 0, 1],
                    [0, 1, 2],
                    [3, 4, 5]]}})


    def test_dressing_faces(self):
        """Test creating a simple impl with 1 node with multiple points and faces sets
        for dressing. This tests both multi-textures and merging of faces with the same texture"""
        room = concrete_room.ConcreteRoom()
        self.assertIsNotNone(room)
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        parent.add_dressing_faces(
            [ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) , cgtypes.vec3(3) ],
            [ [0,1,2], [1,2,3], [2,3,0] ],
            concrete_room.get_texture_definition("myfilename"))
        parent.add_dressing_faces(
            [  cgtypes.vec3(2) , cgtypes.vec3(3) , cgtypes.vec3(4) ],
            [ [0,1,2] ],
            concrete_room.get_texture_definition("myfilename"))
        parent.add_dressing_faces(
            [ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) , cgtypes.vec3(3) ],
            [ [0,1,2], [1,2,3], [2,3,0]],
            concrete_room.get_texture_definition("myfilename2"))
        j = json.loads(room.dump_to_json())
        faces = j["objects"][0]["dressing"]
        self.assertEqual(faces, {
            'myfilename': {
                'points': [
                    {'x': 0.0, 'y': 0.0, 'z': 0.0, 'u': 0.0, 'v': 0.0},
                    {'x': 1.0, 'y': 1.0, 'z': 1.0, 'u': 1.0, 'v': 1.0},
                    {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 2.0, 'v': 2.0},
                    {'x': 3.0, 'y': 3.0, 'z': 3.0, 'u': 3.0, 'v': 3.0},
                    {'x': 4.0, 'y': 4.0, 'z': 4.0, 'u': 4.0, 'v': 4.0}],
                'faces': [
                    [0, 1, 2], [1, 2, 3], [2, 3, 0], [2, 3, 4]]},
            'myfilename2': {
                'points': [
                    {'x': 0.0, 'y': 0.0, 'z': 0.0, 'u': 0.0, 'v': 0.0},
                    {'x': 1.0, 'y': 1.0, 'z': 1.0, 'u': 1.0, 'v': 1.0},
                    {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 2.0, 'v': 2.0},
                    {'x': 3.0, 'y': 3.0, 'z': 3.0, 'u': 3.0, 'v': 3.0}],
                'faces': [
                    [0, 1, 2], [1, 2, 3], [2, 3, 0]
            ]}}
        )

    def test_dressing_face_with_function(self):
        """Test creating a simple impl with 1 node with multiple points and faces sets
        for dressing. This tests both multi-textures and merging of faces with the same texture"""
        room = concrete_room.ConcreteRoom()
        self.assertIsNotNone(room)
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        parent.add_dressing_faces(
            [ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) , cgtypes.vec3(3) ],
            [ [0,1,2], [1,2,3] ],
            get_texture_definition_with_function("my_filename_toto", my_helper_mapper, 1))
        parent.add_dressing_faces(
            [  cgtypes.vec3(2) , cgtypes.vec3(3) , cgtypes.vec3(4) ],
            [ [0,1,2] ],
            get_texture_definition_with_function("my_filename_toto", my_helper_mapper, 1))
        parent.add_dressing_faces(
            [ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) , cgtypes.vec3(3) ],
            [ [0,1,2], [1,2,3], [2,3,0]],
            get_texture_definition_with_function("my_filename_toto2", my_helper_mapper, 1))
        j = json.loads(room.dump_to_json())
        faces = j["objects"][0]["dressing"]
        print(json.dumps(faces, indent=4, sort_keys=True))
        self.assertEqual(faces,
            {
                "my_filename_toto": {
                 "faces": [ [ 0,1,2], [3,4,5], [6,7,8] ],
                 'points': [
                        {'x': 0.0, 'y': 0.0, 'z': 0.0, 'u': 1.0, 'v': 1.0},
                        {'x': 1.0, 'y': 1.0, 'z': 1.0, 'u': 2.0, 'v': 2.0},
                        {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 3.0, 'v': 3.0},
                        {'x': 1.0, 'y': 1.0, 'z': 1.0, 'u': 2.0, 'v': 2.0},
                        {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 3.0, 'v': 3.0},
                        {'x': 3.0, 'y': 3.0, 'z': 3.0, 'u': 4.0, 'v': 4.0},
                        {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 3.0, 'v': 3.0},
                        {'x': 3.0, 'y': 3.0, 'z': 3.0, 'u': 4.0, 'v': 4.0},
                        {'x': 4.0, 'y': 4.0, 'z': 4.0, 'u': 5.0, 'v': 5.0}]
            }, "my_filename_toto2": {
                "faces": [ [ 0,1,2], [3,4,5], [6,7,8] ],
                'points': [
                        {'x': 0.0, 'y': 0.0, 'z': 0.0, 'u': 1.0, 'v': 1.0},
                        {'x': 1.0, 'y': 1.0, 'z': 1.0, 'u': 2.0, 'v': 2.0},
                        {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 3.0, 'v': 3.0},
                        {'x': 1.0, 'y': 1.0, 'z': 1.0, 'u': 2.0, 'v': 2.0},
                        {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 3.0, 'v': 3.0},
                        {'x': 3.0, 'y': 3.0, 'z': 3.0, 'u': 4.0, 'v': 4.0},
                        {'x': 2.0, 'y': 2.0, 'z': 2.0, 'u': 3.0, 'v': 3.0},
                        {'x': 3.0, 'y': 3.0, 'z': 3.0, 'u': 4.0, 'v': 4.0},
                        {'x': 0.0, 'y': 0.0, 'z': 0.0, 'u': 1.0, 'v': 1.0}],
            }})

    def test_dressing_face_with_simple_mapper(self):
        """Test creating a simple impl with 1 node with multiple points and faces sets
        for dressing. This tests both multi-textures and merging of faces with the same texture"""
        room = concrete_room.ConcreteRoom()
        self.assertIsNotNone(room)
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        parent.add_dressing_faces(
            [
             cgtypes.vec3(0.0, 0.0, 0.0), # O
             cgtypes.vec3(1.0, 0.0, 0.0), # Ox
             cgtypes.vec3(0.0, 1.0, 0.0), # Oy
             cgtypes.vec3(0.0, 0.0, 1.0)  # Oz
            ],
            [ [0, 1, 2, 3], [0, 2, 1], [0, 3, 1] ],
            gltf_helper.get_texture_definition_function_simple_mapper("my_filename_toto"))
        j = json.loads(room.dump_to_json())
        faces = j["objects"][0]["dressing"]
        print(json.dumps(faces, indent=4, sort_keys=True))
        self.assertEqual(faces,
            {
    "my_filename_toto": {
        "faces": [
            [ 0,1,2,3], [4,5,6], [7,8,9]
        ],
        "points": [
            { "u": 0.0, "v": 0.0, "x": 0.0, "y": 0.0, "z": 0.0 },
            { "u": 1.0, "v": 0.0, "x": 1.0, "y": 0.0, "z": 0.0 },
            { "u": 0.0, "v": 0.0, "x": 0.0, "y": 1.0, "z": 0.0 },
            { "u": 0.0, "v": 1.0, "x": 0.0, "y": 0.0, "z": 1.0 },
            { "u": 0.0, "v": 0.0, "x": 0.0, "y": 0.0, "z": 0.0 },
            { "u": 1.0, "v": 0.0, "x": 0.0, "y": 1.0, "z": 0.0 },
            { "u": 0.0, "v": 1.0, "x": 1.0, "y": 0.0, "z": 0.0 },
            { "u": 0.0, "v": 0.0, "x": 0.0, "y": 0.0, "z": 0.0 },
            { "u": 1.0, "v": 0.0, "x": 0.0, "y": 0.0, "z": 1.0 },
            { "u": 0.0, "v": 1.0, "x": 1.0, "y": 0.0, "z": 0.0 }
        ]
    }
}

)


    def test_dump_objects(self):
        """ test dumping hierachy of nodes """
        room = concrete_room.ConcreteRoom()
        parent1 = room.add_child(None, "parent1")
        parent2 = room.add_child(None, "parent2")
        child1_1 = room.add_child("parent1", "child1_1")
        child1_2 = room.add_child("parent1", "child1_2")
        child2_1 = room.add_child("parent2", "child2_1")
        child2_2 = room.add_child("parent2", "child2_2")
        self.assertIsNotNone(parent1)
        self.assertIsNotNone(parent2)
        self.assertIsNotNone(child2_1)
        self.assertIsNotNone(child2_2)
        self.assertIsNotNone(child1_1)
        self.assertIsNotNone(child1_2)
        room.generate_gltf("/tmp")
        with open("/tmp/room.gltf", "r") as room_file:
            obj = json.load(room_file)
        objects = obj["nodes"]
        self.assertEqual(objects,
        [
            {'children': [1, 2]},
            {'name': 'parent1', 'children': [3, 4] },
            {'name': 'parent2', 'children': [5, 6] },
            {'name': 'child1_1'},
            {'name': 'child1_2'},
            {'name': 'child2_1'},
            {'name': 'child2_2'}
        ])

    def test_dump_objects_matrix(self):
        """ test dumping hierachy of nodes """
        room = concrete_room.ConcreteRoom()
        parent1 = room.add_child(None, "parent1")
        child1_1 = room.add_child("parent1", "child1_1", cgtypes.mat4(
                        1.0, 2.0, 3.0, 4.0,
                        5.0, 6.0, 7.0, 8.0,
                        9.0, 10.0, 11.0, 12.0,
                        13.0, 14.0, 15.0, 16.0))
        self.assertIsNotNone(parent1)
        self.assertIsNotNone(child1_1)
        room.generate_gltf("/tmp")
        with open("/tmp/room.gltf", "r") as room_file:
            obj = json.load(room_file)
        objects = obj["nodes"]
        self.assertEqual(objects,
        [
            {'children': [1]},
            {'name': 'parent1', 'children': [2]},
            {'name': 'child1_1', 'matrix': [
              1.0,              5.0,              9.0,              0,
              2.0,              6.0,              10.0,              0,
              3.0,              7.0,              11.0,              0,
              4.0,              8.0,              12.0,              1
              ],},
        ])

    def test_proj_matrix(self):
        """test proj matrix construction"""
        default_tex = concrete_room.get_texture_definition(
                "texture.png")
        self.assertEqual(default_tex,
        {
            'texture': 'texture.png',
            'proj': cgtypes.mat4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                1.0, 1.0, 1.0, 0.0)})
        offset_tex = concrete_room.get_texture_definition(
                "texture.png", offset = cgtypes.vec3(1,2,3))
        self.assertEqual(offset_tex,
        {
            'texture': 'texture.png',
            'proj': cgtypes.mat4(
                1.0, 0.0, 0.0, 1.0,
                0.0, 1.0, 0.0, 2.0,
                0.0, 0.0, 0.0, 3.0,
                1.0, 1.0, 1.0, 0.0)})
        scale_tex = concrete_room.get_texture_definition(
                "texture.png", scale = 2.0)
        self.assertEqual(scale_tex,
        {
            'texture': 'texture.png',
            'proj': cgtypes.mat4(
                2.0, 0.0, 0.0, 0.0,
                0.0, 2.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                1.0, 1.0, 1.0, 0.0)})
        axes_tex = concrete_room.get_texture_definition(
                "texture.png", axes=[ ["x","y"], [ "z"] ])
        self.assertEqual(axes_tex,
        {
            'texture': 'texture.png',
            'proj': cgtypes.mat4(
                1.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                1.0, 1.0, 1.0, 0.0)})

    def test_merge(self):
        """
        test merging 2 concrete rooms. 2nd structure is linked to first
        by the name of one of its node's parent child.
        """
        default_tex = concrete_room.get_texture_definition(
                "texture.png")
        room1 = concrete_room.ConcreteRoom()
        parent1 = room1.add_child(None, "parent1")
        child1_1 = room1.add_child("parent1", "child1_1")
        child1_2 = room1.add_child("parent1", "child1_2")
        index0 = child1_1.add_structure_points([ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ])
        self.assertEqual(0, index0)
        child1_1.add_structure_faces(
            index0,
            [ [0,1,2]],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], [ 0 ] )

        room2 = concrete_room.ConcreteRoom()
        parent2 = room2.add_child("child1_1", "parent2") # volontarily linked to previous
        child2_1 = room2.add_child("parent2", "child2_1")
        child2_2 = room2.add_child("parent2", "child1_2")
        index0 = child2_1.add_structure_points([ cgtypes.vec3(4), cgtypes.vec3(5), cgtypes.vec3(6) ])
        self.assertEqual(0, index0)
        child2_1.add_structure_faces(
            index0,
            [ [0,1,2]],
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], [ 0 ] )

        room1.merge(room2)
        nodes = room1.get_objects()
        node_names = [ n.name for n in nodes]
        self.assertEqual(node_names, ['parent1', 'child1_1', 'child1_2', 'parent2', 'child2_1', 'child1_2'])
        room1.generate_gltf("/tmp/")
        with open("/tmp/room.gltf", "r") as room_file:
            obj = json.load(room_file)
        objects = obj["nodes"]
        print(objects)
        self.assertEqual(objects,
            [{'children': [1]},
            {'name': 'parent1', 'children': [2, 3]},
            {'name': 'child1_1', 'children': [4], 'extras': {'phys_faces': [{'data': [0], 'accessor': 1}], 'points': 0}},
            {'name': 'child1_2'},
            {'name': 'parent2', 'children': [5, 6]},
            {'name': 'child2_1', 'extras': {'phys_faces': [{'data': [0], 'accessor': 3}], 'points': 2}},
            {'name': 'child1_2'}])

    def test_dump_1_face_3_points(self):
        """ test dumping 1 face with 3 points (aka triangle)  """
        room = concrete_room.ConcreteRoom()
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        parent.add_dressing_faces(
            [cgtypes.vec3(0,0,0),
            cgtypes.vec3(0,0,1),
            cgtypes.vec3(0,1,0) ],
            [ [0,1,2] ],
            concrete_room.get_texture_definition(
                "texture.png",
                axes=[ ["y"],["z"] ],
                scale=1.0 ))
        room.generate_gltf("/tmp")
        with open("/tmp/room.gltf", "r") as room_file:
            obj = json.load(room_file)
        parent_object = obj["nodes"][1]
        print(parent_object)

    def test_dump_cube(self):
        """ test dumping 1 face with 3 points (aka triangle) . Test preview dump """
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

        path_gen = "/tmp/test_cube/output"
        pathlib.Path(path_gen).mkdir(parents=True, exist_ok=True)
        room.generate_gltf(path_gen)
        with open(path_gen + "/room.gltf", "r") as room_file:
            obj = json.load(room_file)
        parent_object = obj["nodes"][1]
        print(parent_object)
        concrete_room.preview(path_gen + "/room.gltf", path_gen + "/room_preview.gltf")


    def test_gravity_1_node(self):
        """Test generating gravity for one node only"""
        room = concrete_room.ConcreteRoom()
        parent1 = room.add_child(None, "parent1")
        parent2 = room.add_child(None, "parent2")
        child2_1 = room.add_child("parent2", "child2_1")
        child2_2 = room.add_child("parent2", "child2_2")
        self.assertIsNotNone(parent1)
        self.assertIsNotNone(parent2)
        self.assertIsNotNone(child2_1)
        self.assertIsNotNone(child2_2)

        child2_1.set_gravity_script("BIDON", [ concrete_room.Node.GRAVITY_SIMPLE ])

        PATH = "/tmp/gravity"
        pathlib.Path(PATH).mkdir(parents=True, exist_ok=True)
        room.generate_gltf(PATH)

        with open(PATH + "/script.lua", "r") as script_file:
            content = script_file.read()

        self.assertTrue("BIDON" in content)
        self.assertTrue("function gravity_child2_1(tab_in)" in content)


    def test_gravity_2_node2(self):
        """Test generating gravity for 2 nodes"""
        room = concrete_room.ConcreteRoom()
        parent1 = room.add_child(None, "parent1")
        parent2 = room.add_child(None, "parent2")
        child2_1 = room.add_child("parent2", "child2_1")
        child2_2 = room.add_child("parent2", "child2_2")
        self.assertIsNotNone(parent1)
        self.assertIsNotNone(parent2)
        self.assertIsNotNone(child2_1)
        self.assertIsNotNone(child2_2)

        child2_1.set_gravity_script("FOO", [ concrete_room.Node.GRAVITY_SIMPLE ])
        child2_2.set_gravity_script("BAR", [ concrete_room.Node.GRAVITY_SIMPLE ])

        PATH = "/tmp/gravity"
        pathlib.Path(PATH).mkdir(parents=True, exist_ok=True)
        room.generate_gltf(PATH)

        with open(PATH + "/script.lua", "r") as script_file:
            content = script_file.read()

        self.assertTrue("FOO" in content)
        self.assertTrue("BAR" in content)
        self.assertTrue("function gravity_child2_1(tab_in)" in content)
        self.assertTrue("function gravity_child2_2(tab_in)" in content)


    def test_trigger_box(self):
        """Test a trigger box"""
        room = concrete_room.ConcreteRoom()
        parent1 = room.add_child(None, "parent1")
        parent2 = room.add_child(None, "parent2")
        self.assertIsNotNone(parent1)
        self.assertIsNotNone(parent2)

        parent1.add_trigger_box(
                    concrete_room.Node.TRIGGER_LEAVE_BOX,
                    concrete_room.Node.TRIGGER_SET_FALSE,
                    cgtypes.vec3(1.0,    2.0,   3.0),
                    cgtypes.vec3(4.0,    5.0,   6.0),
                    "my_event")
        parent1.add_trigger_box(
                    concrete_room.Node.TRIGGER_ENTER_BOX,
                    concrete_room.Node.TRIGGER_SET_TRUE,
                    cgtypes.vec3(7.0,    8.0,   9.0),
                    cgtypes.vec3(10.0,  11.0,  12.0),
                    "my_event_2")

        PATH = "/tmp/triggers"
        pathlib.Path(PATH).mkdir(parents=True, exist_ok=True)
        room.generate_gltf(PATH)

        with open("/tmp/triggers/room.gltf", "r") as room_file:
            obj = json.load(room_file)
        parent_1_object = obj["nodes"][1]
        parent_2_object = obj["nodes"][2]

        print(parent_1_object)
        print(parent_2_object)

        self.assertFalse("extras" in parent_2_object)
        self.assertTrue("extras" in parent_1_object)

        extras = parent_1_object["extras"]
        self.assertTrue("triggers" in extras)
        triggers = extras["triggers"]
        print(triggers)
        self.assertEqual(triggers,
            [
                {
                    'box': [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]],
                    'set_trigger': 'my_event',
                    'is_box_mode_enter': False,
                    'target_value': False
                },
                {
                    'box': [[7.0, 8.0, 9.0], [10.0, 11.0, 12.0]],
                    'set_trigger': 'my_event_2',
                    'is_box_mode_enter': True,
                    'target_value': True
                }
            ])

    def test_set_root_node(self):
        """Test setting the root node for all children that don't have a parent"""
        room = concrete_room.ConcreteRoom()
        parent1 = room.add_child(None, "parent1")
        child1_1 = room.add_child("parent1", "child1_1")
        self.assertIsNotNone(parent1)
        self.assertIsNotNone(child1_1)

        room.set_root("my_parent")
        self.assertEqual(room.objects[0].parent, "my_parent")
        self.assertEqual(room.objects[1].parent, "my_parentparent1")

    def test_append_prefix(self):
        """Test setting a prefix to all nodes"""
        room = concrete_room.ConcreteRoom()
        parent1 = room.add_child(None, "parent1")
        parent2 = room.add_child(None, "parent2")
        child2_1 = room.add_child("parent2", "child2_1")
        child2_2 = room.add_child("parent2", "child2_2")
        self.assertIsNotNone(parent1)
        self.assertIsNotNone(parent2)
        self.assertIsNotNone(child2_1)
        self.assertIsNotNone(child2_2)

        room.append_prefix("prefix:")
        self.assertEqual(room.objects[0].name, "prefix:parent1")
        self.assertEqual(room.objects[1].name, "prefix:parent2")
        self.assertEqual(room.objects[2].name, "prefix:child2_1")
        self.assertEqual(room.objects[3].name, "prefix:child2_2")


    def test_private_data(self):
        """Test add private data"""
        room = concrete_room.ConcreteRoom()


        room.add_private_data({"private_data_0": {"data":0}})
        room.add_private_data({"private_data_1": {"data":1}})

        PATH = "/tmp/private_data"
        pathlib.Path(PATH).mkdir(parents=True, exist_ok=True)
        room.generate_gltf(PATH)

        with open("/tmp/private_data/room.gltf", "r") as room_file:
            obj = json.load(room_file)
        print(json.dumps(obj, indent=1))

        self.assertTrue("extras" in obj)
        extras = obj["extras"]
        self.assertEqual(extras,
            {
                "private_data_0": {"data": 0},
                "private_data_1": {"data": 1}
            })


if __name__ == '__main__':
    unittest.main()
