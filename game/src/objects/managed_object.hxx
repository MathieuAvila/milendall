#pragma once

#include "movable_object.hxx"
#include "glm/mat4x4.hpp"

struct MovementWish {

    /** @brief a requested movement */
    glm::vec3 walk;
};

class ViewableObject;

/** @brief  interface object that represents an object that can be managed by ObjectManager.
 *          It is the root object of any rela object
 */
class ManagedObject
{

    public:

        /** @brief get main parameters of the object and advance it */
        virtual const MovableObjectDefinition& getObjectDefinition() const = 0;

        /** @brief get requested movement.
        * direction and up are copied "as is"
        * position and room are ignored */
        virtual MovementWish getRequestedMovement() const = 0;

        /** @brief Get the local transformation of the object inside its reference frame.
         * The object is free to place itself how it wishes in his own context */
        virtual glm::mat4x4 getOwnMatrix() const = 0;

        /** @brief Check if it must be released. */
        virtual bool checkEol() const = 0;

        /** @brief Get a reference to the associated viewable object. Defaults to nullptr == no viewable */
        virtual std::shared_ptr<ViewableObject> getViewable() const;

        /** @brief to be derived */
        ManagedObject() = default;

        /** @brief needs too be derivable. */
        virtual ~ManagedObject() = default;
};
