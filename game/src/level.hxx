#pragma once

#include "file_library.hxx"

//class Room;
#include "room.hxx"


/** @brief Any exception related to loader */
class LevelException: public std::runtime_error
{
    public:

        LevelException(const std::string err): runtime_error(err) {};
};

class Level
{
    public:

        /** build the level from the path to the final level file.
         * It's up to the called to have a naming scheme for levels */
        Level(FileLibrary::UriReference ref);

    private:

        std::vector<std::shared_ptr<Room>> rooms;
};

