from enum import IntEnum

class LevelState(IntEnum):
    New = 0
    Story = 1
    Instantiated = 2
    Personalized = 3
    DressingInstantiated = 4
    DressingPersonalized = 5
    Finalize = 6

class StateList():

    def __init__(self, filename=None):
        '''If filename is passed, get the list of states from a filename'''
        self.current = []
        if filename != None:
            with open(filename, "r") as input_file:
                word_list = input_file.readline().strip()
            self.current = [LevelState.__members__[name] for name in word_list.split() if name]

    def save(self, filename):
        '''Save to a file'''
        with open(filename, "w") as output_file:
            my_str = " ".join([i.name for i in self.current])
            output_file.write(my_str)

    def __repr__(self):
        return " ".join([i.name for i in self.current])

    def has_state(self, state):
        return state in self.current

    def add_state(self, state):
        if not self.has_state(state):
            self.current.append(state)