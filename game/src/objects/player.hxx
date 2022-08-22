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

    struct ActionSet {
        bool forward;
        bool backward;
        bool right;
        bool left;
        bool jump;
        bool action;
        bool fire;
        float horizontalAngle;
        float verticalAngle;
    };

    virtual const MovableObjectDefinition& getObjectDefinition() const override;
    virtual MovementWish getRequestedMovement() const override;
    virtual bool checkEol() const override;
    virtual glm::mat4x4 getOwnMatrix() const override;


    void setActionSet(ActionSet actions);

    Player();
    virtual ~Player();

    private:

    MovableObjectDefinition movable_definition;
    ActionSet currentActions;

};
