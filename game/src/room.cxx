
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "common.hxx"
#include "room.hxx"
#include "gltf_mesh.hxx"
#include "json_helper_accessor.hxx"
#include "gltf_exception.hxx"

static auto console = spdlog::stdout_color_mt("room");

struct RoomNode : public GltfNode
{
    public:

    RoomNode(nlohmann::json& json, GltfDataAccessorIFace* data_accessor);

};

RoomNode::RoomNode(nlohmann::json& json, GltfDataAccessorIFace* data_accessor) :
        GltfNode(json)
{


    if (json.contains("extras")) {
        auto extras = json["extras"];
        auto points = jsonGetElementByName(extras, "points").get<int>();
        console->info("Found application data (extras) for RoomNode, points are {}", points);
        auto points_accessor = data_accessor->accessId(points);
        if (points_accessor->unit_type != points_accessor->FLOAT)
            throw GltfException("invalid data type, except float");
        if (points_accessor->vec_type != points_accessor->VEC3)
            throw GltfException("invalid unit type, except VEC3 for points");
        float* raw_data = (float*)(points_accessor->data);
        console->info("points count {}", points_accessor->count);
        for (auto i = 0; i < points_accessor->count; i++)
        {
            console->info("point {}, [ {} {} {} ]", i, raw_data[i*3], raw_data[i*3 + 1], raw_data[i*3 + 2]);
        }

        jsonExecuteAllIfElement(extras, "phys_faces", [this, data_accessor](nlohmann::json& phys, int node_index) {
            auto data = jsonGetElementByName(phys, "data");
            auto type = jsonGetElementByName(data, "type").get<string>();
            auto accessor = jsonGetElementByName(phys, "accessor").get<int>();
            console->info("found phys_face:{} with accessor={}, type is {}", to_string(data), accessor, type);
            auto node_data = data_accessor->accessId(accessor);

            uint16_t* raw_data = (uint16_t*)(node_data->data);
        });
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
