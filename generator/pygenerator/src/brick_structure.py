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

    def generic_generate(self, concrete):
        if self._element.values.objects is not None:
            objects = concrete.add_child(None, "objects")
            for object in self._element.values.objects:
                objects.add_object(object.type, object.position, object.parameters)
        self.generate(concrete)

