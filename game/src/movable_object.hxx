#pragma once

#include <memory>
#include <vector>
#include <list>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/** @brief interface object that represents an object.
 * however it can be just used as a message for computing a more complex
 * position/speed
 */
struct MovableObject
{
        /** @brief radius of the object */
        const float radius;
        /** @brief weight, gravity use this */
        const float weight;
        /** @brief how much it is possible to rise when there's a stair */
        const float jumpable_height;

        /** @brief current position, will be updated */
        glm::vec3 position;
        /** @brief direction vector */
        glm::vec3 direction;
        /** @brief up vector */
        glm::vec3 up;
        /** @brief speed vector */
        glm::vec3 speed;

        /** @brief current room the object is in.
         * This may change in case a portal is gone through */
        std::string room;

        MovableObject(float _radius, float _weight, float _jumpable_height):
            radius(_radius), weight(_weight), jumpable_height(_jumpable_height) {};
};
