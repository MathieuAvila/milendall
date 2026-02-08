"""
interface definition for a dressing
"""

from __future__ import annotations

import concrete_room
from typing_defs import ElementWithValues, SelectorLike


class Dressing():

    """
    Gives "interface" to what must be implemented for a dressing
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create dressing
    """

    _name = "Dressing"

    _element: ElementWithValues | None

    def __init__(self, element: ElementWithValues | None = None) -> None:
        """ nothing"""
        self._element = element

    def get_instance(self, element: ElementWithValues | None = None) -> Dressing:
        """ Return instance for a given dressing """
        raise NotImplementedError("subclass me")

    def check_fit(self) -> int:
        """ Check dressing fits. """
        return 100

    def get_class(self) -> str:
        """ get my class for selector"""
        return "dressing"

    def get_name(self) -> str:
        """ name accessor"""
        return self._name

    def instantiate(self, selector: SelectorLike) -> None:
        """ performs parameters selection. Parameters should be enough to generate specific file"""
        raise NotImplementedError("subclass me")

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""
        raise NotImplementedError("subclass me")
