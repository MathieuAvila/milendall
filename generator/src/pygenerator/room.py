from munch import DefaultMunch

class Room:

    def __init__(self, dict):
        self.values = DefaultMunch.fromDict(dict)

    def dump_graph(self, output):

        label = self.values.room_id
        print(label)
        if (self.values.triggers != None):
            label += "<BR/><B>".join(["\n"] + [t.trigger_id for t in self.values.triggers])+"</B>"
        if (self.values.structure_class != None):
            label += "<BR/><I>S: "+ self.values.structure_class + "</I>"
        if (self.values.dressing_class != None):
            label += "<BR/><I>D: "+ self.values.dressing_class + "</I>"
        output.write('"' + self.values.room_id +'" ' + '[ label=< ' + label+ ' > ] ;\n')

    