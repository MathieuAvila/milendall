from room_selector import RoomSelector
from rooms import register

from rooms import *

#import logging

class RoomSelectorRegular(RoomSelector):

    def __init__(self):
        assert("Subclass me")

    def get_room_fit(self, room):

        l = [register.roomTypes[name].get_instance(room) for name in register.roomTypes 
                if register.roomTypes[name].get_instance(room).check_fit()]
        #logging.info("Fit list is: " + str(register.roomTypes))
        return l

    def get_from_name(self, name, room):
        return register.roomTypes[name].get_instance(room)
