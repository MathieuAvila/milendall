
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "common.hxx"
#include "room.hxx"
#include "gltf_mesh.hxx"

static auto console = spdlog::stdout_color_mt("room");

struct RoomNode : public GltfNode
{
    public:

    RoomNode(nlohmann::json& json, GltfDataAccessorIFace* data_accessor);

};

RoomNode::RoomNode(nlohmann::json& json, GltfDataAccessorIFace* data_accessor) :
        GltfNode(json)
{
    if (json.contains("extra")) {
      console->info("Found application data for RoomNode");
    }
}


std::shared_ptr<GltfNode> instantiateRoomNode(
            nlohmann::json& json,
            GltfDataAccessorIFace* data_accessor)
{
    return make_shared<RoomNode>(json, data_accessor);
}

void Room::parseApplicationData(nlohmann::json& json) {
    console->info("Parse application data for room");
}

Room::Room(GltfMaterialLibraryIfacePtr materialLibrary, FileLibrary::UriReference& ref) : GltfModel(materialLibrary, ref, instantiateRoomNode)
{
    console->info("Loaded room: {}", ref.getPath());
    //FileLibrary::UriReference room_ref = ref.getSubPath("room.gltf");
    //model = std::make_unique<GltfModel>(materialLibrary, room_ref);
    //model.reset(new GltfModel(room_ref));
}
