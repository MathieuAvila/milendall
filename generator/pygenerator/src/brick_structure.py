"""
interface definition for a brick structure
"""

from elemental_structure import ElementalStructure

class BrickStructure(ElementalStructure):

    """
    Gives "interface" to what must be implemented for a brick structure
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create bricks
    """

    _name = "BrickStructure"

    def get_class(self):
        """ get my class for selector"""
        return "brick"

    def check_fit(self):
        """check it can be applied. """
        raise "subclass me"
