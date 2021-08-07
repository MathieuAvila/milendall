#pragma once

#include "managed_object.hxx"
#include "movable_object.hxx"

/** @brief interface object that represents an object.
 * however it can be just used as a message for computing a more complex
 * position/speed
 */
class Player : public ManagedObject
{
    public:

    virtual MovableObjectDefinition& getMovableObject() override;
    virtual bool checkEol() override;

    Player();
    virtual ~Player();

    private:

    MovableObjectDefinition movable_definition;

};
