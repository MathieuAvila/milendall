""" define a super-class for finding structure definitions. Useful for tests """

class Selector:

    """ finding and selecting your structure """

    def get_room_structure_fit(self, _room):
        """ find a list of fits for the room """
        raise "Subclass me"

    def get_room_structure_from_name(self, name, _room):
        """ return a room structure from a name"""
        raise "Subclass me"

    def get_gate_structure_fit(self, _gate):
        """ find a list of fits for the gate """
        raise "Subclass me"

    def get_gate_structure_from_name(self, name, _gate):
        """ return a gate structure from a name"""
        raise "Subclass me"
