#include "gravity_information.hxx"
#include <algorithm>
#include "common.hxx"
#include "helper_math.hxx"

static auto console = getConsole("gravity_information");

GravityInformation::GravityInformation():
    gravity(0.0f, -1.0f, 0.0f), up(0.0f, 1.0f, 0.0f), validity(1000.0f), space_kind(SpaceKind::GROUND), weight(0.0f)
{
}

GravityInformation::GravityInformation(glm::vec3 _gravity, glm::vec3 _up, float _validity, float _weight):
 gravity(_gravity), up(_up), validity(_validity), space_kind(SpaceKind::GROUND), weight(_weight)
{

}

GravityInformation::GravityInformation(glm::vec3 _gravity, glm::vec3 _up, float _validity, SpaceKind _space_kind, float _weight):
 gravity(_gravity), up(_up), validity(_validity), space_kind(_space_kind), weight(_weight)
{
}

GravityInformation::GravityInformation(std::list<GravityInformation> sources)
{
    // ATM don't manage different SpaceKind
    space_kind = SpaceKind::GROUND;
    validity = 1000.0f;
    weight = 0.0f;
    if (sources.size()) {
        auto total_gravity = glm::vec3(0.0f);
        auto total_up = glm::vec3(0.0f);
        auto total_weight = 0.0f;
        auto current_validity = validity;
        std::for_each(sources.cbegin(), sources.cend(), [&current_validity, &total_gravity, &total_up, &total_weight](const GravityInformation& c){
            current_validity = std::min(current_validity, c.validity);
            total_gravity += c.weight * c.gravity;
            total_up +=  c.weight * c.up;
            total_weight += c.weight;
        });
        weight = total_weight;
        if (total_weight != 0.0f) {
            up = total_up / total_weight;
            gravity = total_gravity / total_weight;
        } else {
           gravity = glm::vec3(0.0f, -1.0f, 0.0f);
           up = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        // take minimum, whatever current values. This means that there some space requiring more frequent computations.
        validity = current_validity;
        console->info("total_weight {} up {} gravity {} validity {}",
                total_weight, vec3_to_string(up), vec3_to_string(gravity), validity);
    } else{
        gravity = glm::vec3(0.0f, -1.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
    }
}

bool GravityInformation::operator==(const GravityInformation& g) const
{
    return (gravity == g.gravity) && (up == g.up) && (validity == g.validity) && (space_kind == g.space_kind);
}
