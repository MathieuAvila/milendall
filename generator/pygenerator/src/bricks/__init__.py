from __future__ import annotations

import importlib
import os

files: list[str] = os.listdir(os.path.dirname(__file__))
modules: list[str] = [k[:-3] for k in files if '.py' in k]

for _m in modules:
    importlib.import_module("bricks." + _m)

#logger.info("Bricks structure modules are: %s", str(modules))
