from munch import DefaultMunch

from rooms import *

class Room:

    def __init__(self, dict):
        self.values = DefaultMunch.fromDict(dict)

    def dump_graph(self, output):

        output.write('"' + self.values.room_id +'" ' + '[ label="" ] ;\n')


    