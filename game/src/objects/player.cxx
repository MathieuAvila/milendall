#include "player.hxx"


Player::Player() : movable_definition(MovableObjectDefinition(0.7f, 30.0f, 1.0f))
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
    return MovementWish();
}

bool Player::checkEol()
{
    return false;
}

void Player::setActionSet(ActionSet actions)
{
    currentActions = actions;
}
