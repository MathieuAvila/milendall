
import logging
import os
import importlib

files = os.listdir(os.path.dirname(__file__))
modules = [ k[:-3] for k in files if '.py' in k]

for _m in modules:
    importlib.import_module("rooms." + _m)

logging.info("Room structure modules are: %s", str(modules))
