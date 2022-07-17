#pragma once

#include "point_of_view.hxx"
#include "structure_object_type.hxx"

struct FaceHard;

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
         * @param portalCrossed If a portal was crossed
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
            FaceHard*& face,
            bool& portalCrossed
            ) const = 0;

        /** @brief Manage actions when moving an object inside a given POV.
         *         Don't bother managing when there is a change in space.
         * @param previous_position where from
         * @param next_position where to
         * @param object_type What is moving.
         * @param activated If object is pushing the push key.
        */
        virtual void applyTrigger(
            const PointOfView& previous_position,
            const glm::vec3& next_position,
            const STRUCTURE_OBJECT_TYPE object_type,
            const bool activated) const = 0;

        virtual ~SpaceResolver() = default;
};

