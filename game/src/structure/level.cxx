#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "json_helper_accessor.hxx"
#include "common.hxx"
#include "helper_math.hxx"

#include "level.hxx"
#include "room.hxx"
#include "room_resolver.hxx"
#include "states_list.hxx"
#include "impl_room_node_portal_register.hxx"


static auto console = getConsole("level");

struct LevelRoomResolver : public RoomResolver
{
    std::map<std::string, std::shared_ptr<Room>>& rooms;

    /** Used by room to go through portal */
    virtual Room* getRoom(std::string name) { return rooms[name].get();};

    /** Init with ref to room list. This is linked to lifespan of level, so no dangling ptr here */
    LevelRoomResolver(std::map<std::string, std::shared_ptr<Room>>& _rooms) : rooms(_rooms) {};

    virtual ~LevelRoomResolver() {};
};

RoomResolver* Level::getRoomResolver()
{
    return room_resolver.get();
}

Level::Level(FileLibrary::UriReference ref)
{
    console->info("Load level: {}", ref.getPath());
    json j_level = json::parse(ref.readStringContent());
    room_resolver = make_unique<LevelRoomResolver>(rooms);
    portal_register = make_unique<ImplRoomNodePortalRegister>();
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    states_list = make_unique<StatesList>();
    for(auto room_it : jsonGetElementByName(j_level, "rooms")) {
            auto room_id = jsonGetElementByName(room_it, "room_id").get<string>();
            console->info("Found room_id: {}", room_id);
            auto ref_room = ref.getDirPath().getSubPath(room_id+ "/room.gltf");
            auto room = std::make_shared<Room>(room_id, materialLibrary, ref_room, portal_register.get(), states_list.get());
            rooms.insert({room_id, room});
    }
    auto declarations = jsonGetElementByName(j_level, "declarations");
    auto game_type = jsonGetElementByName(declarations, "game_type");
    auto single_mode = jsonGetElementByName(game_type, "single_mode");
    definition.start_room = jsonGetElementByName(single_mode, "entry_point_room").get<string>();
    definition.end_room = jsonGetElementByName(single_mode, "end_point_room").get<string>();
    definition.recommended_time = jsonGetElementByName(single_mode, "execution_time").get<float>();
    definition.start_position = glm::vec3(
        jsonGetElementByIndex(single_mode, "entry_point_position", 0),
        jsonGetElementByIndex(single_mode, "entry_point_position", 1),
        jsonGetElementByIndex(single_mode, "entry_point_position", 2)
    );
}

Level::GlobalDefinition Level::getDefinition()
{
    return definition;
}

std::list<std::string> Level::getRoomNames()
{
    std::list<std::string> result;
    for (const auto& r: rooms) result.push_back(r.first);
    return result;
}

void Level::draw(PointOfView pov)
{
    if (rooms.count(pov.room) == 0)
        throw LevelException("No such room with this ID: " + pov.room);
    auto room = rooms[pov.room];
    room->draw(pov);
}

void Level::update(float elapsed_time)
{
    for (const auto& r: rooms) {
        r.second->updateRoom(elapsed_time);
    }
}

PointOfView Level::getDestinationPov(const PointOfView& origin, const glm::vec3& destination) const
{
    auto room = room_resolver->getRoom(origin.room);
    PointOfView result(origin);
    result.position = destination;
    PointOfView destinationPov(result);
    glm::vec3 changePoint; // ignored
    float distance; // ignored
    GateIdentifier gateId; // ignored
    PointOfView newPovChangePoint, newPovDestination;

    if (room->getDestinationPov(origin, destinationPov, changePoint, distance, newPovChangePoint, newPovDestination, gateId))
        result = newPovDestination;
    return result;
}

bool Level::isWallReached(
            const PointOfView& origin,
            const glm::vec3& destination,
            const float radius,
            PointOfView& endPoint,
            glm::vec3& vectorEndPoint,
            glm::vec3& destinationEndPoint,
            glm::vec3& normal,
            float& distance,
            FaceHard*& face
            ) const
{
    PointOfView current_origin(origin);
    glm::vec3 current_destination(destination);
    bool need_recompute;
    bool wall_hit = false;
    distance = 0.0f;

    int counter = 0;

    do {
        need_recompute = false;
        float current_distance = glm::length(current_destination - current_origin.position);

        // compute portal reach
        auto room = room_resolver->getRoom(current_origin.room);
        PointOfView portal_destinationPov(current_origin);
        portal_destinationPov.position = current_destination;
        glm::vec3 portal_changePoint;
        float portal_distance = current_distance;
        GateIdentifier portal_gateId; // ignored
        PointOfView portal_newPovChangePoint, portal_newPovDestination;
        bool portal_reach = room->getDestinationPov(
            current_origin, portal_destinationPov,
            portal_changePoint, portal_distance,
            portal_newPovChangePoint, portal_newPovDestination,
            portal_gateId);

        // compute wall reach
        float wall_distance = current_distance;
        glm::vec3 wall_changePoint;
        glm::vec3 wall_normal;
        FaceHard * wall_face;
        bool wall_reach = room->isWallReached(
            current_origin.position, current_destination,
            radius, wall_changePoint, wall_normal, wall_distance, wall_face);

        // if the 2 are hit, keep the shortest.
        if (wall_reach && portal_reach) {
            if (wall_distance < portal_distance)
                portal_reach = false;
            else
                wall_reach = false;
        }

        // handle the case where nothing is hit
        if (!wall_reach && !portal_reach) {
            endPoint = current_origin;
            endPoint.position = current_destination;
            destinationEndPoint = endPoint.position;
            vectorEndPoint = glm::normalize(destinationEndPoint - current_origin.position);
            distance += current_distance;
        }

        // handle the case where a wall is hit
        if (wall_reach) {
            endPoint = current_origin;
            endPoint.position = wall_changePoint;
            destinationEndPoint = current_destination;
            vectorEndPoint = glm::normalize(destinationEndPoint - current_origin.position);
            normal = wall_normal;
            distance += wall_distance;
            face = wall_face;
            wall_hit = true;
        }

        // handle the case where a portal is hit
        if (portal_reach) {
            current_origin = portal_newPovChangePoint;
            current_destination = portal_newPovDestination.position;
            vectorEndPoint = glm::normalize(portal_newPovDestination.position - portal_newPovChangePoint.position);
            // need to check in new space
            //console->info("Portal crossed.\nNew origin: {}\nNew destination: {}",
            //        to_string(current_origin), vec3_to_string(current_destination));
            distance += portal_distance;
            need_recompute = true;
        }
        if (counter++ == 100) {
            console->warn("Reached max loop count. This is unusual and probably a bug.");
            break;
        }

    } while (need_recompute);
    return wall_hit;
}

Level::~Level()
{
}

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

GravityInformation Level::getGravityInformation(
            const PointOfView& position,
            glm::vec3 speed,
            float weight,
            float radius,
            float total_time) const
{
    auto room = room_resolver->getRoom(position.room);
    GravityInformation result = room->getGravity(position.position, speed, weight, radius, total_time);
    return result;
}

void Level::applyTrigger(
            const PointOfView& previous_position,
            const glm::vec3& next_position,
            const STRUCTURE_OBJECT_TYPE object_type,
            const bool activated) const
{
    auto room = room_resolver->getRoom(previous_position.room);
    room->applyTrigger(
            previous_position.position,
            next_position,
            object_type,
            activated);
}