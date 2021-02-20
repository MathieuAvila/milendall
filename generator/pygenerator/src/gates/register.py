"""automatically import all gates"""

import logging

gateTypes = {}

def register_gate_type(obj):
    gateTypes[obj.get_name()] = obj
    logging.info("Registering %s", obj.get_name())
