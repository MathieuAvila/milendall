from munch import DefaultMunch

class Gate:

    def __init__(self, dict):
        self.values = DefaultMunch.fromDict(dict)

    def dump_graph(self, output):
        
        dir = 'dir="both"'
        if ("direction" in self.values):
            if (self.values["direction"] == "to"):
                dir = 'dir="forward"'
            if (self.values["direction"] == "from"):
                dir = 'dir="back"'
        
        label = self.values["gate_id"]
        
        if (self.values.structure_class != None):
            label += "<BR/><I>S: "+ self.values.structure_class + "</I>"
        if (self.values.dressing_class != None):
            label += "<BR/><I>D: "+ self.values.dressing_class + "</I>"

        output.write(self.values["from"] +' -> ' + self.values["to"] +
            ' [ label= < ' + label +  ' > ' + 
            dir + '] ;\n')
