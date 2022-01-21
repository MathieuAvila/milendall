"""automatically import all bricks"""

import logging

brickTypes = {}

def register_brick_type(obj):
    """Registering a brick"""
    brickTypes[obj.get_name()] = obj
    logging.info("Registering brick %s", obj.get_name())
