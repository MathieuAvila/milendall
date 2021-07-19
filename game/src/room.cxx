
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "common.hxx"
#include "room.hxx"
#include "gltf_mesh.hxx"
#include "json_helper_accessor.hxx"
#include "gltf_exception.hxx"
#include "level_exception.hxx"

static auto console = spdlog::stdout_color_mt("room");

struct DrawContext {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    std::string room_name;
};
bool GateIdentifier::operator< (const GateIdentifier& b) const
{
    if (gate == b.gate)
        return (from < b.from);
    return gate < b.gate;
};

bool GateIdentifier::operator== (const GateIdentifier& b) const
{
    return (gate == b.gate) && (from == b.from);
};

RoomNode::FacePortal::FacePortal(
    std::shared_ptr<PointsBlock> points,
    std::unique_ptr<GltfDataAccessorIFace::DataBlock> accessor,
    nlohmann::json& json,
    const std::string& room_name) : face(points, move(accessor))
{
    connect[0] = jsonGetElementByIndex(json, "connect", 0).get<string>();
    connect[1] = jsonGetElementByIndex(json, "connect", 1).get<string>();
    gate = jsonGetElementByName(json, "gate").get<string>();
    in = (room_name == connect[0]);
    console->info("Portal from {} connecting {} and {}, is IN={}", room_name, connect[0], connect[1], in);

}

RoomNode::RoomNode(
    nlohmann::json& json,
    GltfDataAccessorIFace* data_accessor,
    RoomResolver* _room_resolver,
    const std::string& room_name) :
        GltfNode(json), room_resolver(_room_resolver)
{
    if (json.contains("extras")) {
        auto extras = json["extras"];
        auto accessor_points = jsonGetElementByName(extras, "points").get<int>();
        console->info("Found application data (extras) for RoomNode, points are {}", accessor_points);
        auto points_accessor = data_accessor->accessId(accessor_points);
        points = make_shared<PointsBlock>(move(points_accessor));

        jsonExecuteAllIfElement(extras, "phys_faces", [this, data_accessor, room_name](nlohmann::json& phys, int node_index) {
            auto data = jsonGetElementByName(phys, "data");
            auto type = jsonGetElementByName(data, "type").get<string>();
            auto accessor = jsonGetElementByName(phys, "accessor").get<int>();
            console->info("found phys_face:{} with accessor={}, type is {}", to_string(data), accessor, type);
            auto faces_data = data_accessor->accessId(accessor);
            if (type == "portal") {
                portals.push_back(FacePortal(points, move(faces_data), data, room_name));
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

std::list<GateIdentifier> RoomNode::getPortalNameList()
{
    std::list<GateIdentifier> result;
    for (auto& p: portals)
        result.push_back(GateIdentifier{p.gate, p.in});
    return result;
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
            [_room_resolver, _room_name](nlohmann::json& json,
            GltfDataAccessorIFace* data_accessor) {
                return make_shared<RoomNode>(json, data_accessor, _room_resolver, _room_name);
            }),
    room_name(_room_name),
    room_resolver(_room_resolver)
{
    instance = make_unique<GltfInstance>(getInstanceParameters());
    /** collect portals list and associated index */
    for (auto i =0; i < nodeTable.size(); i++) {
        auto portalList = dynamic_cast<RoomNode*>(nodeTable[i].get())->getPortalNameList();
        for (auto p : portalList) {
            portalsIndices.insert(std::pair<GateIdentifier, int>(p, i));
            console->info(
                "Add gate info room: {} {} to index {} - total count {}",
                p.gate, p.from, i, portalsIndices.size());
        }
    }
    console->info("Loaded room: {}", ref.getPath());
}

void Room::applyTransform()
{
    glm::mat4 mat_id(1.0);
    applyDefaultTransform(instance.get(), mat_id);
}

void Room::draw(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    //console->debug("Main room draw");
    struct DrawContext drawContext {position, direction, up, room_name};
    GltfModel::draw(instance.get(), &drawContext);
}

void Room::draw(GltfInstance* instance, int index, void* context)
{
    //console->debug("room draw index {}", index);
    GltfModel::draw(instance, index, context);
    auto myNode = dynamic_cast<RoomNode*>(nodeTable[index].get());
    auto instanceNode = instance->getNode(index);
    myNode->draw(instanceNode, *(struct DrawContext*)(context));
}

std::set<GateIdentifier> Room::getGateNameList() const
{
    std::set<GateIdentifier> result;
    for (auto& p : portalsIndices)
        result.insert(p.first);
    return result;
}

std::pair<RoomNode*, GltfNodeInstanceIface*> Room::getGateNode(const GateIdentifier& gate) const
{
    if (portalsIndices.count(gate) == 0)
        throw LevelException("Requested gate does not exist in this room");
    auto i = portalsIndices.find(gate)->second;
    return std::pair<RoomNode*, GltfNodeInstanceIface*>{
        dynamic_cast<RoomNode*>(nodeTable[i].get()),
        dynamic_cast<GltfNodeInstanceIface*>(instance->getNode(i))
        };
}