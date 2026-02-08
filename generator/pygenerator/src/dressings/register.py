"""automatically import all dressing"""

from __future__ import annotations

import logging

from typing_defs import DressingLike, NamedInstanceFactory

logger = logging.getLogger("brick_register")

dressingTypes: dict[str, NamedInstanceFactory[DressingLike]] = {}


def register_dressing_type(obj: NamedInstanceFactory[DressingLike]) -> None:
    """Registering a dressing"""
    dressingTypes[obj.get_name()] = obj
    logger.debug("Registering dressing %s", obj.get_name())
