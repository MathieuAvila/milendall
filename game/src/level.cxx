#include "level.hxx"
#include "room.hxx"

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "json_helper_accessor.hxx"

#include "common.hxx"
#include <helper_math.hxx>

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
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    for(auto room_it : jsonGetElementByName(j_level, "rooms")) {
            auto room_id = jsonGetElementByName(room_it, "room_id").get<string>();
            console->info("Found room_id: {}", room_id);
            auto ref_room = ref.getDirPath().getSubPath(room_id+ "/room.gltf");
            auto room = std::make_shared<Room>(room_id, materialLibrary, ref_room, room_resolver.get());
            rooms.insert({room_id, room});
    }
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
    for (const auto& r: rooms)
        r.second->applyTransform();
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
            PointOfView& destinationEndPoint,
            glm::vec3& normal,
            float& distance,
            FaceHard*& face
            ) const
{
    PointOfView current_origin(origin);
    glm::vec3 current_destination(destination);
    bool need_recompute;
    bool wall_hit = false;

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
            destinationEndPoint = endPoint;
            distance = current_distance;
        }

        // handle the case where a wall is hit
        if (wall_reach) {
            endPoint = current_origin;
            endPoint.position = wall_changePoint;
            destinationEndPoint = current_origin;
            destinationEndPoint.position = current_destination;
            normal = wall_normal;
            distance = wall_distance;
            face = wall_face;
            wall_hit = true;
        }

        // handle the case where a portal is hit
        if (portal_reach) {
            current_origin = portal_newPovChangePoint;
            current_destination = portal_newPovDestination.position;
            // need to check in new space
            //console->info("Portal crossed.\nNew origin: {}\nNew destination: {}",
            //        to_string(current_origin), vec3_to_string(current_destination));
            need_recompute = true;
        }

    } while (need_recompute);
    return wall_hit;
}

Level::~Level()
{
}
