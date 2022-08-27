#pragma once

#include <nlohmann/json.hpp>

class SpaceResolver;
class GravityProvider;
class ViewablesRegistrar;

/** Provide an interface to an object that loads obecjt based on the room file's content */
class IObjectLoader
{
    public:

    /** @brief this one is called by Level to set all iface references to itself */
    virtual void setReferences(
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider,
            std::shared_ptr<ViewablesRegistrar> _viewables_registrar) = 0;

    /** @brief When a new object is read from room */
    virtual void loadObject(std::string room_name, std::string mesh_name, nlohmann::json& root) = 0;

    virtual ~IObjectLoader() = default;
};
