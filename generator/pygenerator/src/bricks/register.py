"""automatically import all bricks"""

from __future__ import annotations

import logging

from typing_defs import NamedInstanceFactory, StructureLike

logger = logging.getLogger("brick_register")

brickTypes: dict[str, NamedInstanceFactory[StructureLike]] = {}


def register_brick_type(obj: NamedInstanceFactory[StructureLike]) -> None:
    """Registering a brick"""
    brickTypes[obj.get_name()] = obj
    logger.debug("Registering brick %s", obj.get_name())
