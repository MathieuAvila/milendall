"""
interface definition of a real brick
"""

from element import Element

from munch import DefaultMunch

class Brick(Element):

    def __init__(self, values):
        self.values = DefaultMunch.fromDict(values)

    def get_class(self):
        """ get my class for selector"""
        return "brick"

    def get_id(self):
        """ return ID depending on type"""
        return self.values.b_id

    def dump_graph(self, output):
        """
        dump a graphviz repr of a brick
        """

        label = self.values.b_id
        if self.values.triggers is not None:
            label += "<BR/><B>".join(["\n"] + [t.trigger_id for t in self.values.triggers])+"</B>"
        if self.values.structure_class is not None:
            label += "<BR/><I>S: "+ self.values.structure_class + "</I>"
        if self.values.dressing_class is not None:
            label += "<BR/><I>D: "+ self.values.dressing_class + "</I>"
        output.write('"' + self.values.b_id +'" ' + '[ label=< ' + label+ ' > ] ;\n')

    def finalize(self, concrete, preview=False):
        """ Perform final generate and dressing on one brick."""
