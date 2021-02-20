"""automatically import all rooms"""

import logging

roomTypes = {}

def registerRoomType(obj):
    roomTypes[obj.get_name()] = obj
    logging.info("Registering %s", obj.get_name())


