
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "common.hxx"
#include "room.hxx"
#include "gltf_mesh.hxx"
#include "json_helper_accessor.hxx"
#include "gltf_exception.hxx"

static auto console = spdlog::stdout_color_mt("room");

RoomNode::FacePortal::FacePortal(
    std::shared_ptr<PointsBlock> points,
    std::unique_ptr<GltfDataAccessorIFace::DataBlock> accessor,
    nlohmann::json& json) : face(points, move(accessor))
{
    connect[0] = jsonGetElementByIndex(json, "connect", 0).get<string>();
    connect[1] = jsonGetElementByIndex(json, "connect", 1).get<string>();
    console->info("Portal connecting {} and {}", connect[0], connect[1]);
}

RoomNode::RoomNode(nlohmann::json& json, GltfDataAccessorIFace* data_accessor, RoomResolver* _room_resolver) :
        GltfNode(json), room_resolver(_room_resolver)
{
    if (json.contains("extras")) {
        auto extras = json["extras"];
        auto accessor_points = jsonGetElementByName(extras, "points").get<int>();
        console->info("Found application data (extras) for RoomNode, points are {}", accessor_points);
        auto points_accessor = data_accessor->accessId(accessor_points);
        points = make_shared<PointsBlock>(move(points_accessor));

        jsonExecuteAllIfElement(extras, "phys_faces", [this, data_accessor](nlohmann::json& phys, int node_index) {
            auto data = jsonGetElementByName(phys, "data");
            auto type = jsonGetElementByName(data, "type").get<string>();
            auto accessor = jsonGetElementByName(phys, "accessor").get<int>();
            console->info("found phys_face:{} with accessor={}, type is {}", to_string(data), accessor, type);
            auto faces_data = data_accessor->accessId(accessor);
            if (type == "portal") {
                portals.push_back(FacePortal(points, move(faces_data), data));
            }
        });
    }
}

void RoomNode::draw(GltfNodeInstanceIface * nodeInstance, DrawContext& roomContext)
{
    for (auto& portal: portals) {
        // Am I in or out ?
        int factorInOut = roomContext.room_name == portal.connect[0] ? 1 : -1;
        for (auto face : portal.face.getFaces()) {
            // check normal is valid
            console->info("check normal for room {} ( {} / {}) , factor {}",
                roomContext.room_name,
                portal.connect[0], portal.connect[1],
                factorInOut);
        }
    }
}

void Room::parseApplicationData(nlohmann::json& json) {
    console->info("Parse application data for room");
}

Room::Room(
    std::string _room_name,
    GltfMaterialLibraryIfacePtr materialLibrary,
    FileLibrary::UriReference& ref,
    RoomResolver* _room_resolver)
    :
    GltfModel(materialLibrary, ref,
            [_room_resolver](nlohmann::json& json,
            GltfDataAccessorIFace* data_accessor) {
                return make_shared<RoomNode>(json, data_accessor, _room_resolver);
            }),
    room_name(_room_name),
    room_resolver(_room_resolver)
{
    instance = make_unique<GltfInstance>(getInstanceParameters());
    console->info("Loaded room: {}", ref.getPath());
}

void Room::applyTransform()
{
    glm::mat4 mat_id(1.0);
    applyDefaultTransform(instance.get(), mat_id);
}

void Room::draw(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    GltfModel::draw(instance.get());


}
