import logging
import os


files = os.listdir(os.path.dirname(__file__))
modules = [ k[:-3] for k in files if '.py' in k]
logging.info("Room structure modules are: " + str(modules))

roomTypes = {}

def registerRoomType(name, obj):
    roomTypes[name] = obj
    logging.info("Registering %s" % name)


