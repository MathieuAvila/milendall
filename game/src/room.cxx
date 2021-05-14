
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "common.hxx"
#include "room.hxx"
#include "gltf_mesh.hxx"

static auto console = spdlog::stdout_color_mt("room");

Room::Room(GltfMaterialLibraryIfacePtr materialLibrary, FileLibrary::UriReference& ref)
{
    console->info("Load room: {}", ref.getPath());
    FileLibrary::UriReference room_ref = ref.getSubPath("room.gltf");
    model = std::make_unique<GltfModel>(materialLibrary, room_ref);
    //model.reset(new GltfModel(room_ref));
};
