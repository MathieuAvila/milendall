#include "player.hxx"
#include <cmath>

#include "helper_math.hxx"

Player::Player() : movable_definition(MovableObjectDefinition(0.7f, 1.0f, 1.0f, 0.3f))
{

}

Player::~Player()
{

}

MovableObjectDefinition& Player::getObjectDefinition()
{
    return movable_definition;
}

MovementWish Player::getRequestedMovement()
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

glm::mat4x4 Player::getOwnMatrix()
{
    return getRotatedMatrix(currentActions.verticalAngle, currentActions.horizontalAngle);
}

bool Player::checkEol()
{
    return false;
}

void Player::setActionSet(ActionSet actions)
{
    currentActions = actions;
}
