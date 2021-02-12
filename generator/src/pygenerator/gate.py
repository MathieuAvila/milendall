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

        output.write(self.values["from"] +' -> ' + self.values["to"] +
            ' [ label="' + self.values["gate_id"] +  '" ' + 
            dir + '] ;\n')
