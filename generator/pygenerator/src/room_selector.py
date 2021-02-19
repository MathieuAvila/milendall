""" define a super-class for finding structure definitions. Useful for tests """

class RoomSelector:

    """ finding and selecting your structure """

    def get_room_fit(self, room):
        """ find a list of fits for the room """
        raise "Subclass me"

    def get_from_name(self, name, room):
        """ return a structure from a name"""
        raise "Subclass me"
