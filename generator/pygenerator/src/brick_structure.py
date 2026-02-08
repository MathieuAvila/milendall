"""
interface definition for a brick structure
"""

from __future__ import annotations

import concrete_room

from elemental_structure import ElementalStructure

from typing_defs import ElementWithValues

class BrickStructure(ElementalStructure):

    """
    Gives "interface" to what must be implemented for a brick structure
    Should never been called here, will assert ("abstract type").

    Also provides helpers to create bricks
    """

    _name = "BrickStructure"

    def get_class(self) -> str:
        """ get my class for selector"""
        return "brick"

    def check_fit(self) -> int:
        """check it can be applied. """
        raise NotImplementedError("subclass me")

    def generic_generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        if self._element is None:
            raise RuntimeError("Structure requires an element to generate")
        if self._element.values.objects is not None:
            objects = concrete.add_child(None, "objects")
            for object in self._element.values.objects:
                objects.add_object(object.type, object.position, object.parameters)
        self.generate(concrete)

