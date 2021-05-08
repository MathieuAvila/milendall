#include "level.hxx"
#include "room.hxx"

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "gltf_table_accessor.hxx"

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

    auto my_scene = jsonGetElementByName(j_level, "scene").get<int>();
    console->info("scene:{}", my_scene);

    auto j_scenes = jsonGetElementByName(j_level, "scenes");
    console->info("scenes count:{}", j_scenes.size());
    auto j_scenes_0 = jsonGetElementByIndex(j_level, "scenes", 0);
    auto scene_child_count = j_scenes_0.size();
    console->info("scene has children:{}", scene_child_count);


}
