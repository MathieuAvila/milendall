#pragma once

#include <nlohmann/json.hpp>

class SpaceResolver;
class GravityProvider;
class ViewablesRegistrar;

/** Provide an interface to an object that loads obecjt based on the room file's content */
class IObjectLoader
{
    public:

    virtual void setReferences(
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider,
            ViewablesRegistrar* _viewables_registrar) = 0;
    virtual void loadObject(std::string room_name, std::string mesh_name, nlohmann::json& root) = 0;
    virtual ~IObjectLoader() = default;
};
