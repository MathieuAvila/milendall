#ifndef LEVEL_HXX
#define LEVEL_HXX

#include "file_library.hxx"

//class Room;
#include "room.hxx"

class Level
{
    public:

        /** build the level from the path to the final level file.
         * It's up to the called to have a naming scheme for levels */
        Level(FileLibrary::UriReference ref);

    private:

        std::list<std::shared_ptr<Room>> rooms;
};

#endif