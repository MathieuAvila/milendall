"""
Real room selector based on catalog 
"""

from room_selector import RoomSelector
from rooms import register

from rooms import *

#import logging

class RoomSelectorRegular(RoomSelector):

    def __init__(self):
        """ subclass me"""
        super().__init__()

    def get_room_fit(self, room):

        list_fit = [register.roomTypes[name].get_instance(room) for name in register.roomTypes
                if register.roomTypes[name].get_instance(room).check_fit()]
        #logging.info("Fit list is: " + str(register.roomTypes))
        return list_fit

    def get_from_name(self, name, room):
        """ return the class from the loaded name"""
        return register.roomTypes[name].get_instance(room)
