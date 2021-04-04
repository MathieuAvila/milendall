"""
interface definition for a dressing
"""

class Dressing():

    """
    Gives "interface" to what must be implemented for a dressing
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create dressing
    """

    _name = "Dressing"

    def __init__(self, element=None):
        """ nothing"""
        self._element = element

    def get_instance(self, element=None):
        """ Return instance for a given dressing """
        raise "subclass me"

    def check_fit(self):
        """ Check dressing fits. """
        return True

    def get_class(self):
        """ get my class for selector"""
        return "dressing"

    def get_name(self):
        """ name accessor"""
        return self._name

    def instantiate(self):
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        raise "subclass me"

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        raise "subclass me"
