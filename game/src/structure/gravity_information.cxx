#include "gravity_information.hxx"
#include <algorithm>

GravityInformation::GravityInformation():
    gravity(0.0f, -1.0f, 0.0f), up(0.0f, 1.0f, 0.0f), validity(1000.0f), space_kind(SpaceKind::GROUND)
{
}

GravityInformation::GravityInformation(glm::vec3 _gravity, glm::vec3 _up, float _validity):
 gravity(_gravity), up(_up), validity(_validity), space_kind(SpaceKind::GROUND)
{

}

GravityInformation::GravityInformation(glm::vec3 _gravity, glm::vec3 _up, float _validity, SpaceKind _space_kind):
 gravity(_gravity), up(_up), validity(_validity), space_kind(_space_kind)
{
}

GravityInformation::GravityInformation(std::list<GravityInformation> sources)
{
    // ATM don't manage different SpaceKind
    space_kind = SpaceKind::GROUND;
    validity = 1000.0f;
    if (sources.size()) {
        gravity = glm::vec3(0.0f);
        up = glm::vec3(0.0f);
        std::for_each(sources.cbegin(), sources.cend(), [this](const GravityInformation& c){
            validity = std::min(validity, c.validity);
            gravity += c.gravity;
            up += c.up;
        });
        if (up != glm::vec3(0.0f))
            up = glm::normalize(up);
        else
            up = glm::vec3(0.0f, 1.0f, 0.0f);
    } else{
        gravity = glm::vec3(0.0f, -1.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
    }
}
