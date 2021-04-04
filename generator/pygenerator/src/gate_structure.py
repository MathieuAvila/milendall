"""
interface definition for a gate structure
"""

from elemental_structure import ElementalStructure

class GateStructure(ElementalStructure):

    """
    Gives "interface" to what's must be implemented for a gate structure
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create gates
    """

    _name = "GateStructure"

    def get_class(self):
        """ get my class for selector"""
        return "gate"

    def check_fit(self):
        """ Pass the gate, and list of gates, check it can be applied. """
        raise "subclass me"
