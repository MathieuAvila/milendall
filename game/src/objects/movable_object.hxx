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

        typedef enum { NONE, LOW, ALL } InteractionLevel;

        /** @brief If the object is able to move. If not, don' compute movement */
        const bool can_move;

        /** @brief Interaction level */
        const InteractionLevel interaction_level;

        /** @brief radius of the object */
        const float radius;

        /** @brief weight, gravity use this */
        const float weight;

        /** @brief roughness indicates how much object "adheres" to a surface.
         * 0.0 means it glides
         * 1.0 means it is rough and stops as soon a wall is reached
         */
        const float roughness;

        /** @brief indicates how fast the object is rotating to follow the "UP" direction */
        const float rotation_speed;

        MovableObjectDefinition(float _radius, float _weight, float _roughness, float _rotation_speed):
            can_move(true), interaction_level(ALL), radius(_radius), weight(_weight), roughness(_roughness), rotation_speed(_rotation_speed){};

        MovableObjectDefinition(bool _can_move, InteractionLevel _interaction_level):
            can_move(_can_move), interaction_level(_interaction_level), radius(0.0), weight(0.0), roughness(0.0), rotation_speed(0.0)
        {};
};
