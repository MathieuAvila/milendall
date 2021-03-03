""" define a super-class for finding structure definitions. Useful for tests """

import logging

class Selector:

    """ finding and selecting your structure """

    classes = {}

    def get_structure_fit(self, element):
        """ find a list of fits for the structure from the real ones """
        element_list = self.classes[element.get_class()]
        list_fit = [element_list[name].get_instance(element)
                    for name in element_list
                if element_list[name].get_instance(element).check_fit()]
        logging.info("Fit list is: %s", str(list_fit))
        return list_fit

    def get_structure_from_name(self, name, element):
        """ return the class from the loaded name"""
        return self.classes[element.get_class()][name].get_instance(element)
