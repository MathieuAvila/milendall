
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

static auto console = getConsole("room");

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
    setViewComponents(draw_context.pov.position, draw_context.pov.getDirection(), draw_context.pov.getUp());
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
    setViewComponents(pov.position, pov.getDirection(), pov.getUp());
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
            PointOfView& newPovChangePoint,
            PointOfView& newPovDestination,
            GateIdentifier& newGate
            )
{
    bool result = false;
    for (auto i = 0; i < nodeTable.size(); i++) {
        RoomNode* roomNode = dynamic_cast<RoomNode*>(nodeTable[i].get());
        GltfNodeInstanceIface* roomNodeInstance = dynamic_cast<GltfNodeInstanceIface*>(instance->getNode(i));

        auto localOrigin = origin.changeCoordinateSystem(origin.room, roomNodeInstance->getInvertedNodeMatrix());
        auto localDestination = destination.changeCoordinateSystem(destination.room, roomNodeInstance->getInvertedNodeMatrix());

        GateIdentifier gate;
        string target_room;
        glm::vec3 changePointLocal;
        float distance;
        bool crossed = roomNode->checkPortalCrossing(localOrigin.position, localDestination.position, target_room, gate, changePointLocal, distance);
        if (crossed) {
            changePoint = roomNodeInstance->getInvertedNodeMatrix() * positionToVec4(changePointLocal);
            /*console->info("Crossed portal at change point local:\n{}\ndistance={}\nportal={}/{}==>{}\nwas from local:{}\nwas going to local:\n",
                vec3_to_string(changePointLocal), distance, gate.gate, gate.from, target_room,
                vec3_to_string(localOrigin.position), vec3_to_string(localDestination.position)
                );*/

            // compute target position
            auto new_room = room_resolver->getRoom(target_room);
            auto [new_node, new_instance] = new_room->getGateNode(gate);
            newPovDestination = localDestination.changeCoordinateSystem(
                target_room,
                new_instance->getNodeMatrix());
            auto localChangePointDestination(localDestination);
            localChangePointDestination.position = changePointLocal;
            newPovChangePoint = localChangePointDestination.changeCoordinateSystem(
                target_room,
                new_instance->getNodeMatrix());
            result = true;
        }
    }

    return result;
}

bool Room::isWallReached(
            const glm::vec3& _origin,
            const glm::vec3& _destination,
            float radius,
            glm::vec3& changePoint,
            glm::vec3& normal,
            float& _distance,
            FaceHard*& face
            )
{
    auto distance = _distance;
    auto origin = positionToVec4(_origin);
    auto destination = positionToVec4(_destination);
    bool result = false;
    for (auto i = 0; i < nodeTable.size(); i++) {
        RoomNode* roomNode = dynamic_cast<RoomNode*>(nodeTable[i].get());
        GltfNodeInstanceIface* roomNodeInstance = dynamic_cast<GltfNodeInstanceIface*>(instance->getNode(i));

        auto localOrigin = roomNodeInstance->getInvertedNodeMatrix() * origin;
        auto localDestination = roomNodeInstance->getInvertedNodeMatrix() * destination;

        glm::vec3 wallChangePoint;
        glm::vec3 wallNormal;
        float distanceWall = distance;
        FaceHard* hitFace;
        bool crossed = roomNode->isWallReached(localOrigin, localDestination, radius, wallChangePoint, wallNormal, distanceWall, hitFace);
        //console->info("crossed {}, distance={}, _distance={}, distanceWall={}", crossed, distance, _distance, distanceWall);

        if (crossed && distanceWall < distance) {
            normal = roomNodeInstance->getNodeMatrix() * vectorToVec4(wallNormal);
            _distance = distanceWall;
            changePoint = roomNodeInstance->getNodeMatrix() * positionToVec4(wallChangePoint);
            face = hitFace;
            //console->info("Hit wall at: {}, distance={}", vec3_to_string(changePoint), distance);
            result = true;
        }
    }
    return result;
}
