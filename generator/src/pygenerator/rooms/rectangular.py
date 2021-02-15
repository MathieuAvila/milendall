from room_structure import RoomStructure
from .register import registerRoomType
import logging

class RectangularRoom(RoomStructure):
    
    _name = "rectangular"

    def __init__(self):
        pass

    def check_fit(self, room, gate_list):
        """ Pass the Room, and list of gates, check it can be applied. """
        logging.info("checking if rectangular fits: always !")
        return True

registerRoomType("rectangular", RectangularRoom())
