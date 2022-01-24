"""automatically import all dressing"""

import logging

logger = logging.getLogger("brick_register")

dressingTypes = {}

def register_dressing_type(obj):
    """Registering a dressing"""
    dressingTypes[obj.get_name()] = obj
    logger.debug("Registering dressing %s", obj.get_name())
