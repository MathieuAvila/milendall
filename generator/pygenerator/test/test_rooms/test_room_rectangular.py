"""
test rectangular room
"""

import logging
import sys
import pathlib

import unittest
import rooms.rectangular as rectangular
import gates.simple_door as simple_door
import room
import gate
import concrete_room

import dressings.basic as basic

logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)

logger = logging.getLogger()
logger.level = logging.DEBUG

def_0 = {
   "room_id": "room0",
   "structure_class": "rectangular",
  }

def_gate_0 = {
   "gate_id": "gate0",
   "structure_class": "rectangular",
}

def_gate_1 = {
   "gate_id": "gate1",
   "structure_class": "rectangular",
}

class TestRoomRectangular(unittest.TestCase):

    def test_generate_0_gate(self):
        """generate one rectangular with no gate"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        rect0 = rectangular.RectangularRoom()
        self.assertIsNotNone(rect0)
        custom_room = room.Room(def_0)
        rect1 = rect0.get_instance(custom_room)
        self.assertIsNotNone(rect1)
        rect1.instantiate_defaults()
        rect1.instantiate()
        concrete = concrete_room.ConcreteRoom()
        rect1.generate(concrete)

        basic1 = basic.DressingBasic()
        basic1.generate(concrete)
        path_gen = "/tmp/test_rectangular_basic_0_gate/output"
        pathlib.Path(path_gen).mkdir(parents=True, exist_ok=True)
        concrete.generate_gltf(path_gen)
        concrete_room.preview(path_gen + "/room.gltf", path_gen + "/room_preview.gltf")

    def test_generate_1_gate(self):
        """generate one rectangular with 1 gate"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)

        custom_room = room.Room(def_0)

        # add 1 gate
        door0 = gate.Gate(def_gate_0)
        door_structure_0 = simple_door.DoorGate(door0)
        door_structure_0.instantiate()
        custom_room.gates.append(door0)

        rect0 = rectangular.RectangularRoom()
        self.assertIsNotNone(rect0)
        rect1 = rect0.get_instance(custom_room)
        self.assertIsNotNone(rect1)

        rect1.instantiate_defaults()
        rect1.instantiate()
        concrete = concrete_room.ConcreteRoom()
        rect1.generate(concrete)

        basic1 = basic.DressingBasic()
        basic1.generate(concrete)
        path_gen = "/tmp/test_rectangular_basic_1_gate/output"
        pathlib.Path(path_gen).mkdir(parents=True, exist_ok=True)
        concrete.generate_gltf(path_gen)
        concrete_room.preview(path_gen + "/room.gltf", path_gen + "/room_preview.gltf")


    def test_generate_2_gate(self):
        """generate one rectangular with 1 gate"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)

        custom_room = room.Room(def_0)

        # add 2 gates
        door0 = gate.Gate(def_gate_0)
        door_structure_0 = simple_door.DoorGate(door0)
        door_structure_0.instantiate()
        custom_room.gates.append(door0)

        door1 = gate.Gate(def_gate_1)
        door_structure_1 = simple_door.DoorGate(door1)
        door_structure_1.instantiate()
        custom_room.gates.append(door1)

        rect0 = rectangular.RectangularRoom()
        self.assertIsNotNone(rect0)
        rect1 = rect0.get_instance(custom_room)
        self.assertIsNotNone(rect1)

        rect1.instantiate_defaults()
        rect1.instantiate()
        concrete = concrete_room.ConcreteRoom()
        rect1.generate(concrete)

        basic1 = basic.DressingBasic()
        basic1.generate(concrete)
        path_gen = "/tmp/test_rectangular_basic_2_gate/output"
        pathlib.Path(path_gen).mkdir(parents=True, exist_ok=True)
        concrete.generate_gltf(path_gen)
        concrete_room.preview(path_gen + "/room.gltf", path_gen + "/room_preview.gltf")


if __name__ == '__main__':
    unittest.main()
