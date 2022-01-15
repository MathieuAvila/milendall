""" define a super-class for finding structure definitions. Useful for tests """

import logging
logger = logging.getLogger("selector")
logger.setLevel(logging.INFO)

class Selector:

    """ finding and selecting your structure and dressing"""

    classes = { "structure": { "brick":{}, "gate":{} }, "dressing": { "brick":{}, "gate":{} } }

    def _get_fit(self, sub_class, element):
        """ find a list of fits for a given subclass from the real ones """
        logger.info("SUB_CLASS = %s", sub_class)
        element_list = sub_class[element.get_class()]
        list_fit = [element_list[name].get_instance(element)
                    for name in element_list
                if element_list[name].get_instance(element).check_fit()]
        return list_fit

    def get_structure_fit(self, element):
        """ find a list of fits for the structure from the real ones """
        return self._get_fit(self.classes["structure"], element)

    def get_structure_from_name(self, name, element):
        """ return the class from the loaded name"""
        return self.classes["structure"][element.get_class()][name].get_instance(element)

    def get_dressing_fit(self, element):
        """ find a list of fits for the dressing from the real ones """
        return self._get_fit(self.classes["dressing"], element)

    def get_dressing_from_name(self, name, element):
        """ return the class from the loaded name"""
        return self.classes["dressing"][element.get_class()][name].get_instance(element)

    def get_random_choice(self, l):
        """Random number wrapper, to allow UT control over random-itude."""
        raise "subclass me"

    def get_random_int(self, min,max):
        """Random number wrapper, to allow UT control over random-itude."""
        raise "subclass me"

    def get_random_float(self, min,max):
        """Random number wrapper, to allow UT control over random-itude."""
        raise "subclass me"
