#pragma once

#include <string>

class Room;

struct RoomResolver
{
    virtual Room* getRoom(std::string) = 0;
    virtual ~RoomResolver() = default;
};
