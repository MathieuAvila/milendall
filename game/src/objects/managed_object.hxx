#pragma once

#include "movable_object.hxx"

/** @brief  interface object that represents an object that can be managed by ObjectManager.
 *          It is the root object of any rela object
 */
class ManagedObject
{

    public:

        // get main parameters of the object and advance it
        virtual MovableObjectDefinition& getMovableObject() = 0;

        // Check if it must be released.
        virtual bool checkEol() = 0;

        // to be derived
        ManagedObject() = default;

        // needs too be derivable.
        virtual ~ManagedObject() = default;
};
