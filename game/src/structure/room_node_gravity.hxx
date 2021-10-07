#pragma once

#include "gravity_provider.hxx"
#include "min_max_box.hxx"

#include <nlohmann/json.hpp>
#include <string>

class Script;


/** @brief Provide an interface definition for a class that gives gravity information
 * In real code this is the Level. In UT it can be subclassed to whatever impl.
 * Goal is to avoid inclusion of Level by ObjectManager
 */
class RoomNodeGravity
{
    std::string name;
    Script* room_script;
    MinMaxBox gravity_box;

    GravityProvider::GravityInformation constValues;
    enum { NONE, CONST, SCRIPT} provider_type;

    public:

        /** @brief Return the forces that must be applied to the object
         * @param position where is the object in the local space
         * @param speed used to compute friction
         * @param weight obvious
         * @param radius obvious
         * @param total_time the total time from the start of the game.
         * @param gravity result. Filled with value if return is TRUE.
         * @result TRUE if applicable and gravity is filled, FALSE otherwise.
        */
        virtual bool getGravityInformation(
            glm::vec3 position,
            glm::vec3 speed,
            float weight,
            float radius,
            float total_time,
            GravityProvider::GravityInformation& gravity) const;

        RoomNodeGravity(std::string _name, Script* _room_script); /* will default to true */

        /** @brief Read parameters if passed. May throw if unappplicable */
        void readParameters(nlohmann::json& json);

        virtual ~RoomNodeGravity() = default;
};

