"""
interface definition for a gate structure
"""

class GateStructure:

    """
    Gives "interface" to what's must be implemented for a gate structure
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create gates
    """

    _name = "GateStructure"

    def __init__(self, gate:None):
        """ nothing"""
        self._gate = gate

    def get_instance(self, gate:None):
        """ Return instance for a given gate """
        raise "subclass me"

    def get_name(self):
        """ name accessor"""
        return self._name

    def check_structure(self):
        """ Pass the gate, and list of gates, check everything is as expected.
            Should at least check that the gates are taken into account and at the right sizes
        """
        raise "subclass me"

    def check_fit(self):
        """ Pass the gate, and list of gates, check it can be applied. """
        raise "subclass me"

    def instantiate(self):
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        raise "subclass me"

    def create_structure(self):
        """ create from instance """
        raise "subclass me"


    def load_structure_file(self, filename):
        """ load structure from file """
        raise "subclass me"

    def save_structure_file(self, filename):
        """ dump all to file """
        raise "subclass me"
