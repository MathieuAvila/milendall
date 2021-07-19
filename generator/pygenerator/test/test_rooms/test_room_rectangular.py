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
import selector_regular

import dressings.basic as basic

logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)

logger = logging.getLogger()
logger.level = logging.DEBUG

def_0 = {
   "room_id": "room0",
   "structure_class": "rectangular",
  }

# in
def_gate_0 = {
   "gate_id": "gate0",
   "structure_class": "simple_door",
   "connect" : ["room0", "room1"]
}

# out
def_gate_1 = {
   "gate_id": "gate1",
   "structure_class": "simple_door",
   "connect" : ["room2", "room0"]
}

# in
def_gate_2 = {
   "gate_id": "gate2",
   "structure_class": "simple_door",
   "connect" : ["room0", "room3"]
}

# in
def_gate_3 = {
   "gate_id": "gate3",
   "structure_class": "simple_door",
   "connect" : ["room0", "room4"]
}

class TestRoomRectangular(unittest.TestCase):

    selector = selector_regular.SelectorRegular()

    def test_generate_0_gate(self):
        """generate one rectangular with no gate"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)
        custom_room = room.Room(def_0)
        custom_room.instantiation(self.selector)

        custom_room.structure = rectangular.RectangularRoom().get_instance(custom_room)
        custom_room.dressing = basic.DressingBasic().get_instance(custom_room)

        custom_room.finalize("/tmp/test_rectangular_basic_0_gate/output", preview=True)


    def test_generate_1_gate(self):
        """generate one rectangular with 1 gate"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)

        custom_room = room.Room(def_0)
        custom_room.instantiation(self.selector)

        # add 1 gate
        door0 = gate.Gate(def_gate_0)
        door0.instantiation(self.selector)
        custom_room.gates.append(door0)

        # finalize
        custom_room.finalize("/tmp/test_rectangular_basic_1_gate/output", preview=True)

    def test_generate_2_gate(self):
        """generate one rectangular with 2 gates"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)

        custom_room = room.Room(def_0)

        # add gate 1
        door0 = gate.Gate(def_gate_0)
        door0.instantiation(self.selector)
        custom_room.gates.append(door0)

        # add gate 2
        door1 = gate.Gate(def_gate_1)
        door1.instantiation(self.selector)
        custom_room.gates.append(door1)

        # instantiante room as gates are done
        custom_room.instantiation(self.selector)

        # finalize
        custom_room.finalize("/tmp/test_rectangular_basic_2_gate/output", preview=True)

    def test_generate_4_gate(self):
        """generate one rectangular with 4 gates"""
        stream_handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(stream_handler)

        custom_room = room.Room(def_0)

        # add gate 1
        door0 = gate.Gate(def_gate_0)
        door0.instantiation(self.selector)
        custom_room.gates.append(door0)

        # add gate 2
        door1 = gate.Gate(def_gate_1)
        door1.instantiation(self.selector)
        custom_room.gates.append(door1)

        # add gate 3
        door2 = gate.Gate(def_gate_2)
        door2.instantiation(self.selector)
        custom_room.gates.append(door2)

        # add gate 4
        door3 = gate.Gate(def_gate_3)
        door3.instantiation(self.selector)
        custom_room.gates.append(door3)

        # instantiante room as gates are done
        custom_room.instantiation(self.selector)

        # finalize
        custom_room.finalize("/tmp/test_rectangular_basic_4_gate/output", preview=True)


if __name__ == '__main__':
    unittest.main()
