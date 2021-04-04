"""
interface definition for a room structure
"""

from elemental_structure import ElementalStructure

class RoomStructure(ElementalStructure):

    """
    Gives "interface" to what's must be implemented for a room structure
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create rooms
    """

    _name = "RoomStructure"

    def get_class(self):
        """ get my class for selector"""
        return "room"

    def check_fit(self):
        """check it can be applied. """
        raise "subclass me"
