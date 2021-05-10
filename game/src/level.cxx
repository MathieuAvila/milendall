#include "level.hxx"
#include "room.hxx"

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "json_helper_accessor.hxx"

#include "common.hxx"

static auto console = spdlog::stdout_color_mt("level");

Level::Level(FileLibrary::UriReference ref)
{
    console->info("Load level: {}", ref.getPath());
    json j_level = json::parse(ref.readStringContent());
    for(auto room_it : jsonGetElementByName(j_level, "rooms")) {
            auto room_id = jsonGetElementByName(room_it, "room_id").get<string>();
            console->info("Found room_id: {}", room_id);
            auto ref_room = ref.getDirPath().getSubPath(room_id+ "/");
            auto room = std::make_shared<Room>(ref_room);
    }
}
