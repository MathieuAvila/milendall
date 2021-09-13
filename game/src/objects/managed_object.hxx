#pragma once

#include "movable_object.hxx"
#include "glm/mat4x4.hpp"

struct MovementWish {
    glm::vec3 impulse;
};

/** @brief  interface object that represents an object that can be managed by ObjectManager.
 *          It is the root object of any rela object
 */
class ManagedObject
{

    public:

        /** @brief get main parameters of the object and advance it */
        virtual MovableObjectDefinition& getObjectDefinition() = 0;

        /** @brief get requested movement.
        * direction and up are copied "as is"
        * position and room are ignored */
        virtual MovementWish getRequestedMovement() = 0;

        /** @brief Get the local transformation of the object inside its reference frame.
         * The object is free to place itself how it wishes in his own context */
        virtual glm::mat4x4 getOwnMatrix() = 0;

        // Check if it must be released.
        virtual bool checkEol() = 0;

        // to be derived
        ManagedObject() = default;

        // needs too be derivable.
        virtual ~ManagedObject() = default;
};
