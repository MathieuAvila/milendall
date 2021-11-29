#include "../common.hxx"
#include "managed_object_instance.hxx"
#include "space_resolver.hxx"
#include "gravity_provider.hxx"
#include "helper_math.hxx"

static auto console = getConsole("managed_object_instance");

ManagedObjectInstance::ManagedObjectInstance(
    std::shared_ptr<ManagedObject> _object,
    PointOfView _mainPosition,
    SpaceResolver* _spaceResolver,
    GravityProvider* _gravityProvider):
        object(_object),
        mainPosition(_mainPosition),
        wall_adherence(false),
        current_speed(0.0f),
        current_gravity(0.0f, -0.3f, 0.0f),
        current_up(0.0f, 1.0f, 0.0f),
        spaceResolver(_spaceResolver),
        gravityProvider(_gravityProvider),
        gravity_validity(0.0f), // sets this to force recomputation
        last_update(0.0f) // starts counting from there
{
}

PointOfView ManagedObjectInstance::getObjectPosition()
{
    return mainPosition.prependCoordinateSystem(object->getOwnMatrix());
}

void ManagedObjectInstance::computeNextPosition(float total_time)
{
    float delta_time = total_time - last_update;
    updateGravity(total_time, delta_time);
    auto newPos = getComputeNextPosition(delta_time);
    move(newPos, delta_time);
    last_update = total_time;
}

void ManagedObjectInstance::updateGravity(float total_time, float time_delta)
{
    console->debug("Update total_time {} time_delta {} gravity_validity {}",total_time, time_delta, gravity_validity);
    auto& def = object.get()->getObjectDefinition();
    if (gravity_validity < total_time)
    {
        console->debug("Request new values");
        GravityInformation gravity =
            gravityProvider->getGravityInformation(
                mainPosition,
                glm::vec3(0.0f),
                def.weight,
                def.radius,
                total_time);
        current_gravity = gravity.gravity;
        current_up = gravity.up;
        gravity_validity = total_time + gravity.validity;
        console->debug("New validity {}", gravity_validity);
    }
    mainPosition.local_reference = computeRotatedMatrix(
        mainPosition.local_reference,
        current_up,
        std::function<float(float)>([time_delta, def](float originalAngle)
        {
            if (originalAngle > 0.0f)
                return min(originalAngle, 3.14f * def.rotation_speed * time_delta);
            else
                return max(originalAngle, -3.14f * def.rotation_speed * time_delta);
        } ));
}

glm::vec3 ManagedObjectInstance::getComputeNextPosition(float time_delta) const
{
    glm::vec3 speed_req(0.0f);

    auto _current_speed = current_speed;

    if (wall_adherence) {
        // apply requested movement
        _current_speed = glm::vec3(0.0f);
        auto walk = object.get()->getRequestedMovement().walk;
        speed_req = mainPosition.local_reference * walk;
    }

    // apply gravity
    speed_req += _current_speed + current_gravity;

    // cap speed
    // auto speed_norm = glm::length(speed_req);

    // apply delta_time
    speed_req *= time_delta;

    auto newPos = mainPosition.position + speed_req;

    return newPos;
}

void ManagedObjectInstance::move(glm::vec3 newPos, float time_delta)
{
    bool wall_bounced;

    wall_adherence = false;
    auto previous_position = mainPosition;

    float total_distance = 0.0f;
    glm::vec3 move_vector(0.0f);

    int counter = 0;
    do {
        PointOfView endPoint;
        glm::vec3 vectorEndPoint, destinationEndPoint;
        glm::vec3 normal;
        float distance;
        FaceHard* face;

        auto previous_position = mainPosition;

        // Stop if wall is reached
        wall_bounced = spaceResolver->isWallReached(
            mainPosition,
            newPos,
            object.get()->getObjectDefinition().radius,
            endPoint,vectorEndPoint,destinationEndPoint,
            normal,
            distance,
            face);
        total_distance += distance;
        mainPosition = endPoint;
        if (wall_bounced) {
            newPos = moveOnPlane(endPoint.position, destinationEndPoint, normal, 0.00001f);
            mainPosition.position += normal * 0.00001f;
            wall_adherence = wall_adherence || checkAdherenceCone(normal, current_gravity, 0.5f);
        }
        if (distance != 0.0f)
            move_vector = vectorEndPoint;

        console->debug("wall_bounced={} distance={} move_vector={}", wall_bounced, distance, vec3_to_string(move_vector));
        console->debug("\n" "mainPosition = {}\n"
                           "newPos       = {}",
                        vec3_to_string(mainPosition.position),
                        vec3_to_string(newPos));

        if (counter++ == 20) {
            console->warn("Reached max loop count. This is unusual and probably a bug.");
            break;
        }

    } while (wall_bounced);

    console->debug("total_distance={} move_vector={}", total_distance, vec3_to_string(move_vector));

    // compute new speed
    current_speed = (move_vector * total_distance) / time_delta;

    // if room changed, re-init gravity
    if (previous_position.room != mainPosition.room) {
        gravity_validity = 0.0f;
        console->debug("room changed {} => {}, re-init gravity", previous_position.room, mainPosition.room);
    }

    // Once this is done, advertize to the space
    if (previous_position.room == mainPosition.room) {
        spaceResolver->applyTrigger(
            previous_position, mainPosition.position,
            STRUCTURE_OBJECT_TYPE::OBJECT_PLAYER, // TODO
            false // TODO
            );
    }

}
