from __future__ import annotations

from copy import deepcopy
from typing import Any


def merge(base: Any, override: Any) -> Any:
    """Deep merge override into base (override wins)."""
    if override is None:
        return deepcopy(base)
    if base is None:
        return deepcopy(override)
    if isinstance(base, dict) and isinstance(override, dict):
        result = deepcopy(base)
        for key, value in override.items():
            result[key] = merge(result.get(key), value)
        return result
    if isinstance(base, list) and isinstance(override, list):
        return deepcopy(override)
    return deepcopy(override)
