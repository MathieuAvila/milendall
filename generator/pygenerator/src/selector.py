""" define a super-class for finding structure definitions. Useful for tests """

from __future__ import annotations

from typing import Sequence, TypeVar, TypedDict

import logging

from typing_defs import DressingLike, ElementWithValues, NamedInstanceFactory, SelectableInstance, StructureLike
logger = logging.getLogger("selector")
logger.setLevel(logging.INFO)

TSelectable = TypeVar("TSelectable", bound=SelectableInstance)


class SelectorClasses(TypedDict):
    structure: dict[str, dict[str, NamedInstanceFactory[StructureLike]]]
    dressing: dict[str, dict[str, NamedInstanceFactory[DressingLike]]]


class Selector:

    """ finding and selecting your structure and dressing"""

    classes: SelectorClasses = {
        "structure": { "brick":{}, "gate":{} },
        "dressing": { "brick":{}, "gate":{} },
    }

    def _get_fit(self, sub_class: dict[str, dict[str, NamedInstanceFactory[TSelectable]]],
                 element: ElementWithValues) -> list[TSelectable]:
        """ find a list of fits for a given subclass from the real ones """
        #logger.info("SUB_CLASS = %s", sub_class)
        element_list = sub_class[element.get_class()]
        list_fit = [element_list[name].get_instance(element)
                    for name in element_list
                if element_list[name].get_instance(element).check_fit()]
        return list_fit

    def get_structure_fit(self, element: ElementWithValues) -> list[StructureLike]:
        """ find a list of fits for the structure from the real ones """
        return self._get_fit(self.classes["structure"], element)

    def get_structure_from_name(self, name: str | None, element: ElementWithValues) -> StructureLike:
        """ return the class from the loaded name"""
        if name is None:
            raise ValueError("Structure name cannot be None")
        struct_class = self.classes["structure"]
        search_class = element.get_class()
        if search_class not in struct_class.keys():
            raise Exception("No %s in class, found: %s" % (search_class, struct_class.keys()))
        my_class = self.classes["structure"][element.get_class()]
        if name not in my_class.keys():
            raise Exception("No %s in class list, found: %s" % (name, my_class.keys()))
        return my_class[name].get_instance(element)

    def get_dressing_fit(self, element: ElementWithValues) -> list[DressingLike]:
        """ find a list of fits for the dressing from the real ones """
        return self._get_fit(self.classes["dressing"], element)

    def get_dressing_from_name(self, name: str | None, element: ElementWithValues) -> DressingLike:
        """ return the class from the loaded name"""
        if name is None:
            raise ValueError("Dressing name cannot be None")
        return self.classes["dressing"][element.get_class()][name].get_instance(element)

    def get_random_choice(self, items: Sequence[DressingLike]) -> DressingLike:
        """Random number wrapper, to allow UT control over random-itude."""
        raise NotImplementedError("subclass me")

    def get_random_int(self, min: int, max: int) -> int:
        """Random number wrapper, to allow UT control over random-itude."""
        raise NotImplementedError("subclass me")

    def get_random_float(self, min: float, max: float) -> float:
        """Random number wrapper, to allow UT control over random-itude."""
        raise NotImplementedError("subclass me")

    def load_level_extensions(self, level_directory: str) -> None:
        """Load extension content for a specific level directory."""
        return None
