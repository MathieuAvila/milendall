"""automatically import all bricks"""

import logging

logger = logging.getLogger("brick_register")

brickTypes = {}

def register_brick_type(obj):
    """Registering a brick"""
    brickTypes[obj.get_name()] = obj
    logger.debug("Registering brick %s", obj.get_name())
