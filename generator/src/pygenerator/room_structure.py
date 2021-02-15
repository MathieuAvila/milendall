class RoomStructure:

    """
    Gives "interface" to what's must be implemented for a room structure
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create rooms
    """

    _name = "RoomStructure"

    def __init__(self, room:None):
        """ nothing"""
        self._room = room

    def get_name(self):
        return self._name

    def check_structure(self, gate_list):
        """ Pass the Room, and list of gates, check everything is as expected.
            Should at least check that the gates are taken into account and at the right sizes
        """
        assert("Subclass me")

    def check_fit(self, room, gate_list):
        """ Pass the Room, and list of gates, check it can be applied. """
        assert("Subclass me")

    def instantiate(self, gate_list):
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        assert("Subclass me")

    def create_structure(self):
        """ create from instance """

    def load_structure_file(self, filename):
        """ load structure from file """

    def save_structure_file(self, filename):
        """ dump all to file """
