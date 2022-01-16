"""
interface definition for room/gate element
"""

import logging

logging.basicConfig()
logger = logging.getLogger("element")
logger.setLevel(logging.INFO)

class Element():

    """
    Gives "interface" to what's must be implemented for an element (a gate or room)
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create subclasses
    """


    def personalization(self, selector):
        """ 1. Sort element types that matches constraints: list of gates with format of each.
            Note: it's up to the element type to check criterias
            2. Associate weights for each
            3. Random selection of one type"""

        # start with structure

        logger.info("No need to select class for element: %s", self.get_id())
        self.structure = selector.get_structure_from_name(
                self.values.parameters.structure_class,
                self)
        if self.structure is None:
            logger.error("Unknown class name: %s", self.values.parameters.structure_class)
            raise Exception ("Void fit list for element: " + self.get_id())
        self.structure.instantiate_defaults()
        logger.info("Run instantiation structure parameters for element: %s", self.get_id())
        self.structure.instantiate(selector)

        # same for dressing

        if self.values.parameters.dressing_class is None:
            logger.info("Need to select dressing class for element: %s", self.get_id())
            fit_list = selector.get_dressing_fit(self)
            logger.info("Fit list is: %s", str([a.get_name() for a in fit_list]))
            if fit_list == []:
                logger.error("Fit list is void, cannot choose any dressing."
                              "This is unrecoverable.")
                raise Exception ("Void fit list for element: " + self.get_id())
            # just random selection for the moment
            choice = selector.get_random_choice(fit_list)
            logger.info("Chosen dressing %s for element: %s", choice.get_name() ,self.get_id())
            self.values.parameters.dressing_class = choice.get_name()
        else:
            logger.info("No need to select dressing  class for element: %s", self.get_id())
        self.dressing = selector.get_dressing_from_name(
                self.values.parameters.dressing_class,
                self)
        if self.dressing is None:
            logger.error("Unknown dressing class name: %s", self.values.parameters.dressing_class)
            raise Exception ("Void fit list for element: " + self.get_id())

        if self.values.dressing_private is None:
            self.values.dressing_private = {}
            logger.info("Create private parameters for element: %s", self.get_id())
        if self.values.dressing_parameters is None:
            self.values.dressing_parameters = {}
            logger.info("Create structure parameters for element: %s", self.get_id())

        self.values.dressing_private.update(self.values.dressing_parameters)
        logger.info("Run instantiation dressing parameters for element: %s", self.get_id())
        self.dressing.instantiate(selector)
