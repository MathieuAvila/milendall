#include "object_option.hxx"


ObjectOption::ObjectOption(std::string room_name, std::string mesh_name, nlohmann::json &root)
{
}

bool ObjectOption::checkEol()
{
    return false;
}

MovementWish ObjectOption::getRequestedMovement()
{
    return MovementWish();
}

glm::mat4x4 ObjectOption::getOwnMatrix()
{
    return glm::mat4x4();
}

static MovableObjectDefinition option_def(0.5, 0.0, 0.0, 0.0);

MovableObjectDefinition &ObjectOption::getObjectDefinition()
{
    return option_def;
}
