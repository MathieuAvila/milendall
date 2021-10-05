#pragma once

#include "file_library.hxx"
#include "gltf/gltf_material_accessor_library_iface.hxx"
#include "room.hxx"
#include "level_exception.hxx"

/** @brief Provide an interface definition for a class that gives gravity information
 * In real code this is the Level. In UT it can be subclassed to whatever impl.
 * Goal is to avoid inclusion of Level by ObjectManager
 */
class GravityProvider
{
    public:

        /** @brief Result of gravity computation */
        struct GravityInformation {
            /** @brief direction and force of gravity */
            glm::vec3 gravity;
            /** @brief direction and force of main force. This can be the same as gravity, or otherwise */
            glm::vec3 force;
            /** @brief Duration of validity. 0 means it must always be recomputed, -1 means forever
             * (no recomputation needed) */
            float validity;
        };

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

