#include "level.hxx"
#include "room.hxx"

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "json_helper_accessor.hxx"

#include "common.hxx"

static auto console = spdlog::stdout_color_mt("level");

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

PointOfView Level::getDestinationPov(const PointOfView& origin, const glm::vec3& destination)
{
    auto room = room_resolver->getRoom(origin.room);
    PointOfView result(origin);
    result.position = destination;
    PointOfView destinationPov(result);
    glm::vec3 changePoint; // ignored
    float distance; // ignored
    GateIdentifier gateId; // ignored
    PointOfView newPov = origin;
    if (room->getDestinationPov(origin, destinationPov, changePoint, distance, newPov, gateId))
        result = newPov;
    return result;
}

Level::~Level()
{
}
