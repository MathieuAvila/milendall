""" Definition of a gate"""

from munch import DefaultMunch

class GateSpec():

    def __init__(self, values):
        self.values = DefaultMunch.fromDict(values)

    def get_id(self):
        """ return ID depending on type"""
        return self.values.gate_id

    def dump_graph(self, output):
        """ dump graph data to output"""
        direction = 'dir="both"'
        if "direction" in self.values:
            if self.values["direction"] == "to":
                direction = 'dir="forward"'
            if self.values["direction"] == "from":
                direction = 'dir="back"'

        label = self.values["gate_id"]

        if self.values.structure_class is None:
            label += "<BR/><I>S: "+ self.values.structure_class + "</I>"
        if self.values.dressing_class is None:
            label += "<BR/><I>D: "+ self.values.dressing_class + "</I>"

        output.write(self.values["from"] +' -> ' + self.values["to"] +
            ' [ label= < ' + label +  ' > ' +
            direction + '] ;\n')
