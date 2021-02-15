from room_selector import RoomSelector
from rooms import register
from rooms import *
import logging

class RoomSelectorRegular(RoomSelector):

    def __init__(self):
        assert("Subclass me")

    def get_room_fit(self, room, gate_list):
        l = [register.roomTypes[name] for name in register.roomTypes if register.roomTypes[name].check_fit(room, gate_list)]
        logging.info("Fit list is: " + str(register.roomTypes))
        return l

    def instantiate(self, Room, gate_list):
        assert("Subclass me")
