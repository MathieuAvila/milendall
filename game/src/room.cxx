
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "common.hxx"
#include "room.hxx"
#include "gltf_frame.hxx"

static auto console = spdlog::stdout_color_mt("room");

Room::Room(FileLibrary::UriReference& ref)
{
    console->info("Load room: {}", ref.getPath());
    FileLibrary::UriReference room_ref = ref.getSubPath("room.gltf");
    GltfModel* model = new GltfModel(room_ref);
    //model.reset(new GltfModel(room_ref));
};
