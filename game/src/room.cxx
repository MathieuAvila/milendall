
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "common.hxx"
#include "room.hxx"
#include "gltf_mesh.hxx"
#include "json_helper_accessor.hxx"
#include "gltf_exception.hxx"
#include "level_exception.hxx"

#include "helper_math.hxx"
#include "gl_init.hxx"

#include "point_of_view.hxx"
#include "room_draw_context.hxx"

static auto console = spdlog::stdout_color_mt("room");

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

void Room::draw(DrawContext& draw_context)
{
    //console->info("Room draw: {} - level={}", room_name, draw_context.recurse_level);
    setActiveFbo(&draw_context.fbo);
    setViewComponents(draw_context.pov.position, draw_context.pov.direction, draw_context.pov.up);
    GltfModel::draw(instance.get(), &draw_context);
}

void Room::draw(PointOfView pov)
{
    struct DrawContext drawContext {
        pov,
        room_resolver,
        0,
        FboIndex{0,0}
    };
    setViewComponents(pov.position, pov.direction, pov.up);
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
        throw LevelException(string("Requested gate ") + gate.gate + " " + (gate.from? "from":"to") + " does not exist in room:" + room_name);
    auto i = portalsIndices.find(gate)->second;
    return std::pair<RoomNode*, GltfNodeInstanceIface*>{
        dynamic_cast<RoomNode*>(nodeTable[i].get()),
        dynamic_cast<GltfNodeInstanceIface*>(instance->getNode(i))
        };
}

bool Room::getDestinationPov(
            const PointOfView& origin,
            const PointOfView& destination,
            glm::vec3& changePoint,
            float& distance,
            PointOfView& newPov,
            GateIdentifier& newGate
            )
{
    bool result = false;
    for (auto i = 0; i < nodeTable.size(); i++) {
        RoomNode* roomNode = dynamic_cast<RoomNode*>(nodeTable[i].get());
        GltfNodeInstanceIface* roomNodeInstance = dynamic_cast<GltfNodeInstanceIface*>(instance->getNode(i));


       // auto localOrigin = origin.changeCoordinateSystem(origin.room, roomNodeInstance->getNodeMatrix());
       // auto localDestination = destination.changeCoordinateSystem(destination.room, roomNodeInstance->getNodeMatrix());

        auto localOrigin = origin.changeCoordinateSystem(origin.room, roomNodeInstance->getInvertedNodeMatrix());
        auto localDestination = destination.changeCoordinateSystem(destination.room, roomNodeInstance->getInvertedNodeMatrix());


        GateIdentifier gate;
        string target_room;
        glm::vec3 changePoint;
        float distance;
        bool crossed = roomNode->checkPortalCrossing(localOrigin.position, localDestination.position, target_room, gate, changePoint, distance);
        if (crossed) {
            changePoint = roomNodeInstance->getInvertedNodeMatrix() * positionToVec4(changePoint);
            console->info("Crossed portal at: {}, distance={}, portal={}/{}==>{}",
                vec3_to_string(changePoint), distance, gate.gate, gate.from, target_room);

            // compute target position
            auto new_room = room_resolver->getRoom(target_room);
            auto [new_node, new_instance] = new_room->getGateNode(gate);
            newPov = localDestination.changeCoordinateSystem(
                target_room,
                new_instance->getNodeMatrix());
            result = true;
        }
    }

    return result;
}