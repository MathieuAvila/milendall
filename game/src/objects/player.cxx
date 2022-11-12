#include "player.hxx"
#include <cmath>

#include "helper_math.hxx"
#include "common.hxx"

static auto console = getConsole("player");

Player::Player() :
    movable_definition(MovableObjectDefinition(0.7f, 1.0f, 1.0f, 1.0f)),
    time_left(0.0f),
    exited(false)
{
    previous_vertical_angle = 0.0f;
}

Player::~Player()
{
}

const MovableObjectDefinition& Player::getObjectDefinition() const
{
    return movable_definition;
}

MovementWish Player::getRequestedMovement() const
{
    MovementWish wish;

    glm::mat3 ownMatrix = getOwnMatrix();
    glm::vec3 left = ownMatrix * glm::vec3(2.0 , 0.0, 0.0); // I really don't know what I'm doing...
    glm::vec3 direction = ownMatrix * glm::vec3(0.0 , 0.0, 2.0);
    glm::vec3 up = glm::vec3(0.0 , 5.0, 0.0);

    wish.walk = glm::vec3(0.0f);
    wish.walk += currentActions.forward ? direction : glm::vec3();
    wish.walk += currentActions.backward ? -direction : glm::vec3();
    wish.walk += currentActions.right ? -left : glm::vec3();
    wish.walk += currentActions.left ? left : glm::vec3();
    if (currentActions.jump) {
        wish.walk = wish.walk * 2.0f + up;
    }

    return wish;
}

glm::mat4x4 Player::getOwnMatrix() const
{
    return getRotatedMatrix(currentActions.verticalAngle, currentActions.horizontalAngle);
}

bool Player::checkEol() const
{
    return false;
}

void Player::setActionSet(ActionSet actions)
{
    // cap vertical angle
    float v_diff = actions.verticalAngle - previous_vertical_angle;
    previous_vertical_angle = actions.verticalAngle;
    float curr_vangle = currentActions.verticalAngle;
    curr_vangle += v_diff;
    curr_vangle = min<float>(curr_vangle, 3.14 * 3.5 / 10.0);
    curr_vangle = max<float>(curr_vangle, -3.14 * 3.5 / 10.0);
    currentActions = actions;
    currentActions.verticalAngle = curr_vangle;
}

bool Player::addTime(float time)
{
    console->info("Received time {}", time);
    time_left += time;
    return true;
}

void Player::interactExit()
{
    exited = true;
}

void Player::manage(float diff_time)
{
    if (!exited)
        time_left -= diff_time;
}

void Player::getGameStatus(int& _time_left, bool& _exited)
{
    _time_left = time_left;
    _exited = exited;
}
