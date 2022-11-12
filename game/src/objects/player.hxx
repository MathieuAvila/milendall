#pragma once

#include "managed_object.hxx"
#include "movable_object.hxx"

#include <gtest/gtest_prod.h>

/** @brief interface object that represents an object.
 * however it can be just used as a message for computing a more complex
 * position/speed
 */
class Player : public ManagedObject
{
public:
    struct ActionSet
    {
        bool forward = false;
        bool backward = false;
        bool right = false;
        bool left = false;
        bool jump = false;
        bool action = false;
        bool fire = false;
        float horizontalAngle = 0.0f;
        float verticalAngle = 0.0f;
    };

    virtual const MovableObjectDefinition &getObjectDefinition() const override;
    virtual MovementWish getRequestedMovement() const override;
    virtual bool checkEol() const override;
    virtual glm::mat4x4 getOwnMatrix() const override;

    virtual bool addTime(float time) override;
    virtual void interactExit() override;

    void setActionSet(ActionSet actions);

    Player();
    virtual ~Player();

    /** in order to check game play and print on screen.
     * @returns left time & exit status
     */
    void getGameStatus(int& time, bool& _exited);

    void manage(float diff_time) override;

private:
    MovableObjectDefinition movable_definition;
    ActionSet currentActions;
    float time_left = 0.0f;
    bool exited = false;
    float previous_vertical_angle; // needed to cap v angle.


    FRIEND_TEST(PlayerTest, set_actions);
};
