#pragma once

#include "movable_object.hxx"
#include "glm/mat4x4.hpp"
#include "../point_of_view.hxx"
#include "managed_object.hxx"

#include <gtest/gtest_prod.h>

struct FaceHard;
class SpaceResolver;

class ManagedObjectInstance
{
    protected:

        std::shared_ptr<ManagedObject> object;

        /** @brief This is the real position of the center of the object*/
        PointOfView mainPosition;

        /** @brief This is other positions that the object crosses */
        std::list<PointOfView> otherPosition;

        /** @brief If the object has wall adherence */
        bool wall_adherence;

        /** @brief current speed. Norm gives absolute speed */
        glm::vec3 current_speed;

        /** @brief current gravity */
        glm::vec3 current_gravity;

        /** @brief needed to move object in space */
        SpaceResolver* spaceResolver;

        /** @brief get computed next position without walls detection collision */
        glm::vec3 getComputeNextPosition(float time_delta) const;

        /** @brief move object based on computed postion */
        void move(glm::vec3 computed_position, float time_delta);

        /** @brief Update gravity vector if necessary.*/
        void updateGravity(float time_delta);

    public:

        ManagedObjectInstance(std::shared_ptr<ManagedObject> _object, PointOfView _mainPosition, SpaceResolver* _spaceResolver);

        /** @brief Return the corrent POV */
        PointOfView getObjectPosition();

        /** @brief compute next position */
        void computeNextPosition(float time_delta);

    friend class ManagedObjectInstanceTest;
};
