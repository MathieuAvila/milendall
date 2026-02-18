"""
Real room selector based on catalog
"""

from __future__ import annotations

import hashlib
import importlib.util
import os
import random
import sys
from typing import Sequence

import logging

from selector import Selector
from bricks import register as room_register
from dressings import register as dressing_register
from typing_defs import DressingLike

logger = logging.getLogger("selector_regular")
logger.setLevel(logging.INFO)

class SelectorRegular(Selector):

    def __init__(self, level_directory: str | None = None) -> None:
        self.classes["structure"]["brick"] = room_register.brickTypes
        self.classes["dressing"]["brick"] = dressing_register.dressingTypes
        self._loaded_extension_dirs: set[str] = set()

        if level_directory is not None:
            self.load_level_extensions(level_directory)

        #logger.debug("Room structure list is: %s", " ".join(self.classes["structure"]["brick"]) )
        #logger.debug("Room dressing list is: %s", " ".join(self.classes["dressing"]["brick"]) )

    def get_random_choice(self, items: Sequence[DressingLike]) -> DressingLike:
        return random.choice(items)

    def get_random_int(self, min: int, max: int) -> int:
        """Random number wrapper, to allow UT control over random-itude."""
        return random.randint(min,max)

    def get_random_float(self, min: float, max: float) -> float:
        """Random number wrapper, to allow UT control over random-itude."""
        return random.random()* (max-min) + min

    def load_level_extensions(self, level_directory: str) -> None:
        self._load_extension_category(level_directory, "bricks")
        self._load_extension_category(level_directory, "dressings")

    def _load_extension_category(self, level_directory: str, category: str) -> None:
        extensions_dir = os.path.abspath(
            os.path.join(level_directory, "extensions", category)
        )
        if not os.path.isdir(extensions_dir):
            return
        if extensions_dir in self._loaded_extension_dirs:
            return
        self._loaded_extension_dirs.add(extensions_dir)
        namespace = hashlib.md5(extensions_dir.encode("utf-8")).hexdigest()[:8]
        self._load_extension_modules(extensions_dir, namespace, category)

    def _load_extension_modules(self, directory: str, namespace: str, category: str) -> None:
        for filename in sorted(os.listdir(directory)):
            if not filename.endswith(".py") or filename.startswith("__"):
                continue
            module_path = os.path.join(directory, filename)
            module_name = f"extensions.{category}.{namespace}.{filename[:-3]}"
            if module_name in sys.modules:
                continue
            spec = importlib.util.spec_from_file_location(module_name, module_path)
            if spec is None or spec.loader is None:
                raise ImportError(f"Unable to load extension module {module_path}")
            module = importlib.util.module_from_spec(spec)
            sys.modules[module_name] = module
            logger.debug("Loading extension brick module %s", module_path)
            spec.loader.exec_module(module)
