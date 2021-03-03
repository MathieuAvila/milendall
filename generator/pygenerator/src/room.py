"""
interface definition of room
"""

from munch import DefaultMunch

class Room:

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
