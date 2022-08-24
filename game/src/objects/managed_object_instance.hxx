#pragma once

#include "movable_object.hxx"
#include "glm/mat4x4.hpp"
#include "../point_of_view.hxx"
#include "managed_object.hxx"
#include "viewables_registrar.hxx"

#include <gtest/gtest_prod.h>

struct FaceHard;
class SpaceResolver;
class GravityProvider;
class ViewablesRegistrar;

class ManagedObjectInstance
{
    protected:

        std::shared_ptr<ManagedObject> object;

        /** @brief This is the real position of the center of the object*/
        PointOfView mainPosition;

        /** @brief The mesh it is linked to; "" means none. */
        std::string mesh_name;

        /** @brief This is other positions that the object crosses */
        std::list<PointOfView> otherPosition;

        /** @brief If the object has wall adherence */
        bool wall_adherence;

        /** @brief current speed. Norm gives absolute speed */
        glm::vec3 current_speed;

        /** @brief current gravity. This gives the force in which the object is attracted.
         * see current_up for direction.
         */
        glm::vec3 current_gravity;

        /** @brief current up direction. This may be different from gravity */
        glm::vec3 current_up;

        /** @brief needed to move object in space */
        SpaceResolver* spaceResolver;

        /** @brief needed to recompute gravity*/
        GravityProvider* gravityProvider;

        /**@brief needed to output viewables objects */
        ViewablesRegistrar* viewables_registrar;

        /** @brief Absolute time of validity of gravity and up */
        float gravity_validity;

        /** last update time */
        float last_update;

        /** @brief ID if viewable exists */
        ViewablesRegistrar::viewableId viewable_id;

        /** @brief get computed next position without walls detection collision */
        glm::vec3 getComputeNextPosition(float time_delta) const;

        /** @brief move object based on computed postion */
        void move(glm::vec3 computed_position, float time_delta);

        /** @brief Update gravity vector if necessary.*/
        void updateGravity(float total_time, float time_delta);

    public:

        ManagedObjectInstance(
            std::shared_ptr<ManagedObject> _object,
            PointOfView _mainPosition,
            std::string _mesh_name,
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider,
            ViewablesRegistrar* _viewables_registrar
            );

        /** @brief Return the corrent POV */
        PointOfView getObjectPosition();

        /** @brief compute next position */
        void computeNextPosition(float total_time);

        /** @brief accessor to compute interactions */
        void getInteractionParameters(float &radius, MovableObjectDefinition::InteractionLevel &level, PointOfView &pos);

        /** @brief Run interact */
        void interact(ManagedObjectInstance* second_object);

        /** @brief proxy to checkEol. */
        bool checkEol();

        /** @brief update viewable */
        void updateViewable();

        ~ManagedObjectInstance();

    FRIEND_TEST(ManagedObjectInstanceTest, update_gravity_check_validity);
    FRIEND_TEST(ManagedObjectInstanceTest, update_gravity_check_rotation);
};
