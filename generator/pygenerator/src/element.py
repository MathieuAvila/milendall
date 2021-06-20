"""
interface definition for room/gate element
"""

import logging

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

class Element():

    """
    Gives "interface" to what's must be implemented for an element (a gate or room)
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create subclasses
    """


    def instantiation(self, selector):
        """ 1. Sort element types that matches constraints: list of gates with format of each.
            Note: it's up to the element type to check criterias
            2. Associate weights for each
            3. Random selection of one type"""

        # start with structure

        if self.values.structure_class is None:
            logging.info("Need to select class for element: %s", self.get_id())
            fit_list = selector.get_structure_fit(self)
            logging.info("Fit list is: %s", str([a.get_name() for a in fit_list]))
            if fit_list == []:
                logging.error("Fit list is void, cannot choose any structure."
                              "This is unrecoverable.")
                raise Exception ("Void fit list for element: " + self.get_id())
            # just random selection for the moment
            choice = selector.get_random_choice(fit_list)
            logging.info("Chosen structure %s for element: %s",
                            choice.get_name(),
                            self.get_id())
            self.values.structure_class = choice.get_name()
        else:
            logging.info("No need to select class for element: %s", self.get_id())
        self.structure = selector.get_structure_from_name(
                self.values.structure_class,
                self)
        if self.structure is None:
            logging.error("Unknown class name: %s", self.values.structure_class)
            raise Exception ("Void fit list for element: " + self.get_id())
        self.structure.instantiate_defaults()
        logging.info("Run instantiation structure parameters for element: %s", self.get_id())
        self.structure.instantiate(selector)

        # same for dressing

        if self.values.dressing_class is None:
            logging.info("Need to select dressing class for element: %s", self.get_id())
            fit_list = selector.get_dressing_fit(self)
            logging.info("Fit list is: %s", str([a.get_name() for a in fit_list]))
            if fit_list == []:
                logging.error("Fit list is void, cannot choose any dressing."
                              "This is unrecoverable.")
                raise Exception ("Void fit list for element: " + self.get_id())
            # just random selection for the moment
            choice = selector.get_random_choice(fit_list)
            logging.info("Chosen dressing %s for element: %s", choice.get_name() ,self.get_id())
            self.values.dressing_class = choice.get_name()
        else:
            logging.info("No need to select dressing  class for element: %s", self.get_id())
        self.dressing = selector.get_dressing_from_name(
                self.values.dressing_class,
                self)
        if self.dressing is None:
            logging.error("Unknown dressing class name: %s", self.values.dressing_class)
            raise Exception ("Void fit list for element: " + self.get_id())

        if self.values.dressing_private is None:
            self.values.dressing_private = {}
            logging.info("Create private parameters for element: %s", self.get_id())
        if self.values.dressing_parameters is None:
            self.values.dressing_parameters = {}
            logging.info("Create structure parameters for element: %s", self.get_id())

        self.values.dressing_private.update(self.values.dressing_parameters)
        logging.info("Run instantiation dressing parameters for element: %s", self.get_id())
        self.dressing.instantiate(selector)
