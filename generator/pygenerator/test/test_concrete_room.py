"""
test level
"""

import logging
import json

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
        print(points)
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
                    'hint': ['building'],
                    'physics': [0]
                },
                {
                    'faces': [[12, 13, 14], [15, 16, 17]],
                    'category': ['phy'],
                    'hint': ['ceiling'],
                    'physics': [1]
                }
            ])


    def test_dressing_faces(self):
        """Test creating a simple impl with 1 node with multiple points and faces sets
        for dressing"""
        room = concrete_room.ConcreteRoom()
        self.assertIsNotNone(room)
        parent = room.add_child(None, "parent")
        self.assertIsNotNone(parent)
        index0 = parent.add_dressing_points([ cgtypes.vec3(0), cgtypes.vec3(1), cgtypes.vec3(2) ])
        self.assertEqual(0, index0)
        index1 = parent.add_dressing_points([ cgtypes.vec3(3), cgtypes.vec3(4), cgtypes.vec3(5) ])
        self.assertEqual(3, index1)
        parent.add_dressing_faces(
            index1,
            [ [0,1,2], [3,4,5], [6,7,8] ],
            concrete_room.get_texture_definition("myfilename"))
        parent.add_dressing_faces(
            index1,
            [ [9,10,11], [12,13,14]],
            concrete_room.get_texture_definition("myfilename2"))
        j = json.loads(room.dump_to_json())
        points = j["objects"][0]["dressing_points"]
        print(points)
        self.assertEqual(points,
        [{'x': 0.0, 'y': 0.0, 'z': 0.0},
        {'x': 1.0, 'y': 1.0, 'z': 1.0},
        {'x': 2.0, 'y': 2.0, 'z': 2.0},
        {'x': 3.0, 'y': 3.0, 'z': 3.0},
        {'x': 4.0, 'y': 4.0, 'z': 4.0},
        {'x': 5.0, 'y': 5.0, 'z': 5.0}])
        faces = j["objects"][0]["dressing_faces"]
        self.assertEqual(faces,
            [
                {
                    'faces': [[3, 4, 5], [6, 7, 8], [9, 10, 11]],
                    'texture': {'texture': 'myfilename',
                    'proj': {'mlist': [1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0,
                    1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0]}}
                },
                {
                    'faces': [[12, 13, 14], [15, 16, 17]],
                    'texture': {'texture': 'myfilename2',
                    'proj': {'mlist': [1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0,
                    1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0]}}
                }
            ]
        )



if __name__ == '__main__':
    unittest.main()
