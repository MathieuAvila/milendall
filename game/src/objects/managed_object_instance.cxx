#include "../common.hxx"
#include "managed_object_instance.hxx"
#include "space_resolver.hxx"
#include "gravity_provider.hxx"
#include "helper_math.hxx"

static auto console = getConsole("managed_object_instance");

ManagedObjectInstance::ManagedObjectInstance(
    std::shared_ptr<ManagedObject> _object,
    PointOfView _mainPosition,
    std::string _mesh_name,
    SpaceResolver *_spaceResolver,
    GravityProvider *_gravityProvider,
    std::shared_ptr<ViewablesRegistrar> _viewables_registrar) : object(_object),
                                                mainPosition(_mainPosition),
                                                mesh_name(_mesh_name),
                                                wall_adherence(false),
                                                current_speed(0.0f),
                                                current_gravity(0.0f, -0.3f, 0.0f),
                                                current_up(0.0f, 1.0f, 0.0f),
                                                spaceResolver(_spaceResolver),
                                                gravityProvider(_gravityProvider),
                                                viewables_registrar(_viewables_registrar),
                                                gravity_validity(0.0f), // sets this to force recomputation
                                                last_update(0.0f),      // starts counting from there
                                                viewable_id(0)
{
}

PointOfView ManagedObjectInstance::getObjectPosition()
{
    return mainPosition.prependCoordinateSystem(object->getOwnMatrix());
}

void ManagedObjectInstance::computeNextPosition(float total_time)
{
    float delta_time = total_time - last_update;

    // If there is a mesh, then it must be solved at first call.
    if (spaceResolver && (mesh_name != "")) {
        auto mat = spaceResolver->getRoomMeshMatrix(mainPosition.room, mesh_name);
        console->warn("POSITION WAS={}", vec3_to_string(mainPosition.position));
        console->warn("MAT IS={}", mat4x4_to_string(mat));
        mainPosition = mainPosition.changeCoordinateSystem(mainPosition.room, mat);
        console->warn("POSITION IS={}", vec3_to_string(mainPosition.position));
        mesh_name = "";
        updateViewable();
    }

    object.get()->manage(delta_time);
    updateGravity(total_time, delta_time);
    if (object.get()->getObjectDefinition().can_move)
    {
        auto newPos = getComputeNextPosition(delta_time);
        move(newPos, delta_time);
    }
    last_update = total_time;
}

void ManagedObjectInstance::updateViewable()
{
    if (viewables_registrar == nullptr)
        return;

    // check viewable
    if (viewable_id == 0)
    {
        auto viewable = object->getViewable();
        if (viewable.get() != nullptr)
        {
            viewable_id = viewables_registrar->appendViewable(viewable);
            console->info("Found viewable and inserted with ID={}", viewable_id);
        }
    }
    // update if any
    if (viewable_id != 0)
    {
        viewables_registrar->updateViewable(viewable_id, mainPosition);
    }
}

void ManagedObjectInstance::updateGravity(float total_time, float time_delta)
{
    if (!gravityProvider)
        return;
    console->debug("Update total_time {} time_delta {} gravity_validity {}", total_time, time_delta, gravity_validity);
    auto &def = object.get()->getObjectDefinition();
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
                return max(originalAngle, -3.14f * def.rotation_speed * time_delta); }));
}

glm::vec3 ManagedObjectInstance::getComputeNextPosition(float time_delta) const
{
    glm::vec3 speed_req(0.0f);

    auto _current_speed = current_speed;

    if (wall_adherence)
    {
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
    if (!spaceResolver)
        return;

    bool wall_bounced;

    wall_adherence = false;
    auto original_previous_position = mainPosition;

    float total_distance = 0.0f;
    glm::vec3 move_vector(0.0f);
    bool portalCrossedGlobal = false;

    int counter = 0;
    do
    {
        PointOfView endPoint;
        glm::vec3 vectorEndPoint, destinationEndPoint;
        glm::vec3 normal;
        float distance;
        FaceHard *face;

        auto previous_position = mainPosition;

        // Stop if wall is reached
        bool portalCrossedLocal = false;
        wall_bounced = spaceResolver->isWallReached(
            mainPosition,
            newPos,
            object.get()->getObjectDefinition().radius,
            endPoint, vectorEndPoint, destinationEndPoint,
            normal,
            distance,
            face,
            portalCrossedLocal);
        portalCrossedGlobal = portalCrossedGlobal || portalCrossedLocal;
        total_distance += distance;
        mainPosition = endPoint;
        if (wall_bounced)
        {
            newPos = moveOnPlane(endPoint.position, destinationEndPoint, normal, 0.00001f);
            mainPosition.position += normal * 0.00001f;
            wall_adherence = wall_adherence || checkAdherenceCone(normal, current_gravity, 0.5f);
        }
        if (distance != 0.0f)
            move_vector = vectorEndPoint;

        console->debug("wall_bounced={} distance={} move_vector={}", wall_bounced, distance, vec3_to_string(move_vector));
        console->debug("\n"
                       "mainPosition = {}\n"
                       "newPos       = {}",
                       vec3_to_string(mainPosition.position),
                       vec3_to_string(newPos));

        if (counter++ == 20)
        {
            console->warn("Reached max loop count. This is unusual and probably a bug.");
            break;
        }

    } while (wall_bounced);

    console->debug("total_distance={} move_vector={}", total_distance, vec3_to_string(move_vector));

    // compute new speed
    current_speed = (move_vector * total_distance) / time_delta;

    // if room changed, re-init gravity
    if (portalCrossedGlobal)
    {
        gravity_validity = 0.0f;
        console->info("portal crossed {} => {}, re-init gravity", original_previous_position.room, mainPosition.room);
    }

    // Once this is done, advertize to the space
    if (original_previous_position.room == mainPosition.room)
    {
        spaceResolver->applyTrigger(
            original_previous_position, mainPosition.position,
            STRUCTURE_OBJECT_TYPE::OBJECT_PLAYER, // TODO
            false                                 // TODO
        );
    }
}

void ManagedObjectInstance::getInteractionParameters(
    float &radius,
    MovableObjectDefinition::InteractionLevel &level,
    PointOfView &pos)
{
    auto& def = object.get()->getObjectDefinition();
    radius = def.radius;
    level = def.interaction_level;
    pos = mainPosition;
}

void ManagedObjectInstance::interact(ManagedObjectInstance* second_object)
{
    object.get()->interact(second_object->object.get());
}

bool ManagedObjectInstance::checkEol()
{
    return object.get()->checkEol();
}

ManagedObjectInstance::~ManagedObjectInstance()
{
    if (viewable_id != 0)
    {
        viewables_registrar->removeViewable(viewable_id);
    }
}
