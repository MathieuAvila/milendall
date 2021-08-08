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

MovableObjectPosition Player::getRequestedMovement()
{
    return MovableObjectPosition();
}

bool Player::checkEol()
{
    return false;
}
