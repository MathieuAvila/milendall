import logging

roomTypes = {}

def registerRoomType(name, obj):
    roomTypes[name] = obj
    logging.info("Registering %s" % name)


