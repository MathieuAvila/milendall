"""test regular load"""

from __future__ import annotations

import os
import tempfile
import textwrap
import unittest

import selector_regular

from bricks import register as room_register
from dressings import register as dressing_register

class TestSelectorRegular(unittest.TestCase):

    def test_get_list_bricks_regular(self) -> None:
        """ test retrieving list of bricks"""

        _s = selector_regular.SelectorRegular()
        print(room_register.brickTypes)
        self.assertIsNotNone(room_register.brickTypes)
        self.assertTrue("rectangular" in room_register.brickTypes)

    def test_load_extension_bricks(self) -> None:
        """test loading level extension bricks"""
        with tempfile.TemporaryDirectory() as temp_dir:
            bricks_dir = os.path.join(temp_dir, "extensions", "bricks")
            dressings_dir = os.path.join(temp_dir, "extensions", "dressings")
            os.makedirs(bricks_dir, exist_ok=True)
            os.makedirs(dressings_dir, exist_ok=True)
            module_path = os.path.join(bricks_dir, "private_test.py")
            module_content = textwrap.dedent(
                """
                from __future__ import annotations

                from brick_structure import BrickStructure
                from bricks.register import register_brick_type
                import concrete_room
                from typing_defs import ElementWithValues, SelectorLike

                class BrickPrivateTest(BrickStructure):
                    _name = "private_test"

                    def __init__(self, element: ElementWithValues | None = None) -> None:
                        self._element = element

                    def get_instance(self, element: ElementWithValues) -> BrickPrivateTest:
                        return BrickPrivateTest(element)

                    def check_fit(self) -> int:
                        return 1

                    def check_structure(self) -> bool:
                        return True

                    def instantiate(self, selector: SelectorLike) -> None:
                        return None

                    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
                        return None

                register_brick_type(BrickPrivateTest())
                """
            )
            with open(module_path, "w", encoding="utf-8") as module_file:
                module_file.write(module_content)

            dressing_path = os.path.join(dressings_dir, "private_dressing.py")
            dressing_content = textwrap.dedent(
                """
                from __future__ import annotations

                from dressing import Dressing
                from dressings.register import register_dressing_type
                import concrete_room
                from typing_defs import ElementWithValues, SelectorLike

                class DressingPrivateTest(Dressing):
                    _name = "private_dressing"

                    def __init__(self, element: ElementWithValues | None = None) -> None:
                        self._element = element

                    def get_instance(self, element: ElementWithValues | None = None) -> DressingPrivateTest:
                        return DressingPrivateTest(element)

                    def check_fit(self) -> int:
                        return 1

                    def instantiate(self, selector: SelectorLike) -> None:
                        return None

                    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
                        return None

                register_dressing_type(DressingPrivateTest())
                """
            )
            with open(dressing_path, "w", encoding="utf-8") as dressing_file:
                dressing_file.write(dressing_content)

            _s = selector_regular.SelectorRegular()
            _s.load_level_extensions(temp_dir)

            try:
                self.assertIn("private_test", room_register.brickTypes)
                self.assertIn("private_dressing", dressing_register.dressingTypes)
            finally:
                room_register.brickTypes.pop("private_test", None)
                dressing_register.dressingTypes.pop("private_dressing", None)


if __name__ == '__main__':
    unittest.main()
