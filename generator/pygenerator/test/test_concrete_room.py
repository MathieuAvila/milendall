"""
test level
"""

import logging

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
        """Test creating a simple impl with 1 node with multiple points and faces sets"""
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
            [concrete_room.Node.CATEGORY_PHYSICS], [concrete_room.Node.HINT_BUILDING], { 0 })


if __name__ == '__main__':
    unittest.main()
