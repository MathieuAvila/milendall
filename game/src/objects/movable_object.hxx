#pragma once

#include <memory>
#include <vector>
#include <list>
#include <string>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/** @brief interface object that represents the definition of an object.
 * however it can be just used as a message for computing a more complex
 * position/speed
 */
struct MovableObjectDefinition
{
        /** @brief radius of the object */
        const float radius;

        /** @brief weight, gravity use this */
        const float weight;

        /** @brief roughness indicates how much object "adheres" to a surface.
         * 0.0 means it glides
         * 1.0 means it is rough and stops as soon a wall is reached
         */
        const float roughness;

        MovableObjectDefinition(float _radius, float _weight, float _roughness):
            radius(_radius), weight(_weight), roughness(_roughness) {};
};
