"""
interface definition of room
"""

import room
import concrete_room
import pathlib
from element import Element

from munch import DefaultMunch

class Room(Element):

    def __init__(self, values):
        self.values = DefaultMunch.fromDict(values)
        self.gates = []

    def get_class(self):
        """ get my class for selector"""
        return "room"

    def get_id(self):
        """ return ID depending on type"""
        return self.values.room_id

    def dump_graph(self, output):
        """
        dump a graphviz repr of a room
        """

        label = self.values.room_id
        print(label)
        if self.values.triggers is not None:
            label += "<BR/><B>".join(["\n"] + [t.trigger_id for t in self.values.triggers])+"</B>"
        if self.values.structure_class is not None:
            label += "<BR/><I>S: "+ self.values.structure_class + "</I>"
        if self.values.dressing_class is not None:
            label += "<BR/><I>D: "+ self.values.dressing_class + "</I>"
        output.write('"' + self.values.room_id +'" ' + '[ label=< ' + label+ ' > ] ;\n')

    def finalize(self, output_dir, preview=False):
        """ Perform final generate and dressing on one room. This is specific to a room
        and builds all intermediate gates when needed."""

        concrete = concrete_room.ConcreteRoom()
        self.structure.generate(concrete)
        self.dressing.generate(concrete)

        for gate in self.gates:
            if not hasattr(gate, "concrete"):
                gate.concrete = concrete_room.ConcreteRoom()
                gate.structure.generate(gate.concrete)
                gate.dressing.generate(gate.concrete)
            concrete.merge(gate.concrete)

        room_path = output_dir + "/" + self.values.room_id
        pathlib.Path(room_path).mkdir(parents=True, exist_ok=True)
        concrete.generate_gltf(room_path)
        if preview:
            concrete_room.preview(room_path + "/room.gltf", room_path + "/room_preview.gltf")
