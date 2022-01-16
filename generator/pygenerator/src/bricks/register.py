"""automatically import all rooms"""

import logging

brickTypes = {}

def register_room_type(obj):
    """Registering a room"""
    brickTypes[obj.get_name()] = obj
    logging.debug("Registering room %s", obj.get_name())
