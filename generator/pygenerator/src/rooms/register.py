"""automatically import all rooms"""

import logging

roomTypes = {}

def register_room_type(obj):
    """Registering a room"""
    roomTypes[obj.get_name()] = obj
    logging.debug("Registering room %s", obj.get_name())
