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
    std::map<std::string, std::shared_ptr<Room>> rooms;

    /** Used by room to go through portal */
    virtual Room* getRoom(std::string name) { return rooms[name].get();};

    /** Init with ref to room list. This is linked to lifespan of level, so no dangling ptr here */
    LevelRoomResolver(std::map<std::string, std::shared_ptr<Room>> _rooms) : rooms(_rooms) {};

    virtual ~LevelRoomResolver() {};
};

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

void Level::draw(std::string room_id, glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    if (rooms.count(room_id) == 0)
        throw LevelException("No such room with this ID: " + room_id);
    auto room = rooms[room_id];
    room->draw(position, direction, up);
}

void Level::update(float elapsed_time)
{
    for (const auto& r: rooms)
        r.second->applyTransform();
}

Level::~Level()
{
}
