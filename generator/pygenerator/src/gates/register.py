"""automatically import all gates"""

import logging

gateTypes = {}

def register_gate_type(obj):
    """Registering a gate"""
    gateTypes[obj.get_name()] = obj
    logging.debug("Registering gate %s", obj.get_name())
