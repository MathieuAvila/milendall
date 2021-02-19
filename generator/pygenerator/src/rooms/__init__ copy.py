import os
import logging

files = os.listdir(os.path.dirname(__file__))
modules = [ k[:-3] for k in files if '.py' in k]
logging.info("Room structure modules are: " + str(modules))

__all__ =  modules
