#pragma once

#include <nlohmann/json.hpp>

/** Provide an interface to an object that loads obecjt based on the room file's content */
class IObjectLoader
{
    public:
    virtual void loadObject(std::string room_name, std::string mesh_name, nlohmann::json& root) = 0;
    virtual ~IObjectLoader() = default;
};
