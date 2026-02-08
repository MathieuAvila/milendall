"""
interface definition for any structural element
"""

from __future__ import annotations

import logging

import concrete_room
from typing_defs import ElementWithValues, SelectorLike

logging.basicConfig()
logger = logging.getLogger("elemental_structure")
logger.setLevel(logging.INFO)

class ElementalStructure():

    """
    Gives "interface" to what's must be implemented for a structure
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create subclasses
    """

    _name = "structure"

    _element: ElementWithValues | None

    def __init__(self, element: ElementWithValues | None) -> None:
        """ nothing"""
        self._element = element

    def get_class(self) -> str:
        """ get my class for selector"""
        raise NotImplementedError("subclass me")

    def get_instance(self, element: ElementWithValues) -> ElementalStructure:
        """ Return instance for a given gate """
        raise NotImplementedError("subclass me")

    def get_name(self) -> str:
        """ name accessor"""
        return self._name

    def check_structure(self) -> bool:
        """check everything is as expected.
        """
        raise NotImplementedError("subclass me")

    def instantiate(self, selector: SelectorLike) -> None:
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        raise NotImplementedError("subclass me")

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""
        raise NotImplementedError("subclass me")

    def instantiate_defaults(self) -> None:
        """Instantiate default values"""
        if self._element is None:
            raise RuntimeError("Structure requires an element to instantiate defaults")
        if self._element.values.parameters.structure_private is None:
            self._element.values.parameters.structure_private = {}
            logger.debug("Create private parameters for element: %s", self._element.get_id())
        if self._element.values.parameters.structure_parameters is None:
            self._element.values.parameters.structure_parameters = {}
            logger.debug("Create structure parameters for element: %s", self._element.get_id())
        self._element.values.parameters.structure_private.update(self._element.values.parameters.structure_parameters)
