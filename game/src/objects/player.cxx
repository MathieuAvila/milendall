#include "player.hxx"


Player::Player() : movable_definition(MovableObjectDefinition(0.7f, 30.0f, 1.0f))
{

}

Player::~Player()
{

}

MovableObjectDefinition& Player::getMovableObject()
{
    return movable_definition;
}

bool Player::checkEol()
{
    return false;
}
