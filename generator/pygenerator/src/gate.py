""" Definition of a gate"""

from munch import DefaultMunch

from element import Element

class Gate(Element):

    def __init__(self, values):
        self.values = DefaultMunch.fromDict(values)

    def get_class(self):
        """ get my class for selector"""
        return "gate"

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

    def get_dimensions(self):
        """return dimensions used to compute rooms places
        This must have the following format:
          {
            "portal": [x,y],
            "margin": [x,y],
            "floor": [ list of axes in the form 'x0', 'x1', 'y0','y1'
          }
        "portal" gives the overall portal format in terms of width and height
        "margin" specifies the requested margins needed to build the portal,
                 this space is potentially taken but must be filled by security
        "floor" tells which direction is a floor
        Should be set by a gate structure
        """
        return self.values.structure_private["geometry"]
