"""
test level
"""

import logging
import json
import pathlib

import unittest
import concrete_room
import cgtypes.mat4

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

class TestRoomImpl(unittest.TestCase):

    def test_parent_child(self):
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
        print(j)

    def test_structure_faces(self):
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

    def test_get_visual_faces(self):
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

    def test_dressing_faces(self):
        """Test creating a simple impl with 1 node with multiple points and faces sets
        for dressing"""
        room = concrete_room.ConcreteRoom()
        self.assertIsNotNone(room)
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        parent.add_dressing_faces(
            [ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ],
            [ [0,1,2], [3,4,5], [6,7,8] ],
            concrete_room.get_texture_definition("myfilename"))
        parent.add_dressing_faces(
            [ cgtypes.vec3(3), cgtypes.vec3(4), cgtypes.vec3(5) ],
            [ [9,10,11], [12,13,14]],
            concrete_room.get_texture_definition("myfilename2"))
        j = json.loads(room.dump_to_json())
        faces = j["objects"][0]["dressing"]
        print(faces)
        self.assertEqual(faces,
            [{
            'points':
            [{'x': 0.0, 'y': 0.0, 'z': 0.0},
            {'x': 1.0, 'y': 1.0, 'z': 1.0},
            {'x': 2.0, 'y': 2.0, 'z': 2.0}],
            'faces': [[0, 1, 2], [3, 4, 5], [6, 7, 8]],
            'texture': {
                'texture': 'myfilename',
                'proj': {'mlist': [
                    1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0]}
                }
            },
            {'points':
            [{'x': 3.0, 'y': 3.0, 'z': 3.0},
            {'x': 4.0, 'y': 4.0, 'z': 4.0},
            {'x': 5.0, 'y': 5.0, 'z': 5.0}],
            'faces': [[9, 10, 11], [12, 13, 14]],
            'texture': {
                'texture': 'myfilename2',
                'proj': {'mlist': [
                    1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0]}
                }
            }]
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
            {'name': 'parent1', 'children': [3, 4], 'mesh': 0},
            {'name': 'parent2', 'children': [5, 6], 'mesh': 1},
            {'name': 'child1_1', 'mesh': 2},
            {'name': 'child1_2', 'mesh': 3},
            {'name': 'child2_1', 'mesh': 4},
            {'name': 'child2_2', 'mesh': 5}
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
                0.0, 0.0, 0.0, 0.0)})
        offset_tex = concrete_room.get_texture_definition(
                "texture.png", offset = cgtypes.vec3(1,2,3))
        self.assertEqual(offset_tex,
        {
            'texture': 'texture.png',
            'proj': cgtypes.mat4(
                1.0, 0.0, 0.0, 1.0,
                0.0, 1.0, 0.0, 2.0,
                0.0, 0.0, 0.0, 3.0,
                0.0, 0.0, 0.0, 0.0)})
        scale_tex = concrete_room.get_texture_definition(
                "texture.png", scale = 2.0)
        self.assertEqual(scale_tex,
        {
            'texture': 'texture.png',
            'proj': cgtypes.mat4(
                2.0, 0.0, 0.0, 0.0,
                0.0, 2.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0)})
        axes_tex = concrete_room.get_texture_definition(
                "texture.png", axes=[ ["x","y"], [ "z"] ])
        self.assertEqual(axes_tex,
        {
            'texture': 'texture.png',
            'proj': cgtypes.mat4(
                1.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0)})

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

if __name__ == '__main__':
    unittest.main()