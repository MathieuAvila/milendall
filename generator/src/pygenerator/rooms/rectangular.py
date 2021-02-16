from room_structure import RoomStructure
from .register import registerRoomType
import logging

class RectangularRoom(RoomStructure):
    
    _name = "rectangular"

    def __init__(self, room=None):
        self.room = room
        pass

    def get_instance(self, room:None):
        return RectangularRoom(room)

    def check_fit(self):
        """ Pass the Room, and list of gates, check it can be applied. """
        logging.info("checking if rectangular fits: always ! rectangular rules the world !")
        return True

    def instantiate(self):
        """ force set values:
        - set values to room size
        - set values for gates"""
        self.room.values.private_parameters={}
        self.room.values.private_parameters["size"] = [10.0,10.0,2.5]
        for gate in self.room.gates:
            logging.info("My gate")


registerRoomType("rectangular", RectangularRoom())
