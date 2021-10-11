#pragma once

#include "glm/vec3.hpp"

#include "file_library.hxx"
#include "point_of_view.hxx"
#include "gravity_information.hxx"

/** @brief Provide an interface definition for a class that gives gravity information
 * In real code this is the Level. In UT it can be subclassed to whatever impl.
 * Goal is to avoid inclusion of Level by ObjectManager
 */
class GravityProvider
{
    public:

        /** @brief Return the forces that must be applied to the object
         * @param position where is the object. Only room and position is used.
         * @param speed used to compute friction
         * @param weight the weight of the object
         * @param radius the radius of the object
         * @param total_time the total time from the start of the game.
         *                   This must be provided although it could be computed.
        */
        virtual GravityInformation getGravityInformation(
            const PointOfView& position,
            glm::vec3 speed,
            float weight,
            float radius,
            float total_time) const = 0;

        virtual ~GravityProvider() = default;
};

