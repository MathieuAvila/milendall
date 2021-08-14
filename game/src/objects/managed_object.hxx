#pragma once

#include "movable_object.hxx"


struct MovementWish {
    glm::vec3 impulse;
};

/** @brief  interface object that represents an object that can be managed by ObjectManager.
 *          It is the root object of any rela object
 */
class ManagedObject
{

    public:

        // get main parameters of the object and advance it
        virtual MovableObjectDefinition& getObjectDefinition() = 0;

        // get requested movement.
        // direction and up are copied "as is"
        // position and room are ignored
        virtual MovementWish getRequestedMovement() = 0;

        // Check if it must be released.
        virtual bool checkEol() = 0;

        // to be derived
        ManagedObject() = default;

        // needs too be derivable.
        virtual ~ManagedObject() = default;
};
