#pragma once

#include "file_library.hxx"
#include "gltf/gltf_material_accessor_library_iface.hxx"
#include "room.hxx"
#include "level_exception.hxx"

/** @brief Provide an interface definition for a class that allows to move objects and detect wall collision
 * In real code this is the Level. In UT it can be subclassed to whatever impl.
 * Goal is to avoid inclusion of Level by ObjectManager
 */
class SpaceResolver
{
    public:

        /** Get a POV if a portal is crossed. This is used to draw in the right POV, as draw POV is higher (head) than
         * the player's sphere radius.
         * @param origin Is the original POV
         * @param destination is the desintation position in the origin's POV space
         * @result the final POV. It may be different than the one passed as origin.
         */
        virtual PointOfView getDestinationPov(const PointOfView& origin, const glm::vec3& destination) const = 0;

        /** @brief Check if a wall is reached. It checks if it goes through a portal,
         *         and makes the appropriate POV translation. So that the POV is returned and
         *         must always be considered as the "good" position
         * @param origin is the original position, ie. start of trajectory
         * @param destination is the end of trajectory
         * @param radius is the size of the object
         * @param endPoint is ALWAYS filled with the destination point, either where the wall is reached or the end portal.
         * @param vectorEndPoint is ALWAYS filled with the move vector in the final POV.
         * @param destinationEndPoint is ALWAYS filled with the destination point in the final POV.
         * @param normal is filled with the normal of the wall
         * @param distance is filled with the distance, if a wall is reached
         * @param face face that was hit
         * @return True if a wall was reached, if ever
        */
        virtual bool isWallReached(
            const PointOfView& origin,
            const glm::vec3& destination,
            const float radius,
            PointOfView& endPoint,
            glm::vec3& vectorEndPoint,
            glm::vec3& destinationEndPoint,
            glm::vec3& normal,
            float& distance,
            FaceHard*& face
            ) const = 0;

        virtual ~SpaceResolver() = default;
};

