
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "common.hxx"
#include "room.hxx"
#include "gltf_mesh.hxx"
#include "json_helper_accessor.hxx"
#include "gltf_exception.hxx"
#include "level_exception.hxx"
#include "room_animation.hxx"
#include "viewables_registrar.hxx"
#include "viewable_object.hxx"

#include "helper_math.hxx"
#include "gl_init.hxx"

#include "point_of_view.hxx"
#include "room_draw_context.hxx"

#include "script.hxx"

static auto console = getConsole("room");

void Room::parseApplicationData(nlohmann::json& json) {
    console->info("Parse application data for room");
}

RoomScriptLoader::RoomScriptLoader(FileLibrary::UriReference& roomRef)
{
    loadedScript = nullptr;
    auto ref = roomRef.getDirPath().getSubPath("script.lua");
    if (ref.isFile())
        loadedScript = make_unique<Script>(roomRef.getDirPath().getSubPath("script.lua"));
    else
        console->info("No script found for room");
}

Script* RoomScriptLoader::getScript()
{
    return loadedScript.get();
}

RoomScriptLoader::~RoomScriptLoader() {};

Room::Room(
    std::string _room_name,
    GltfMaterialLibraryIfacePtr materialLibrary,
    FileLibrary::UriReference& ref,
    IRoomNodePortalRegister* _portal_register,
    StatesList* _states_list,
    ViewablesRegistrar* _viewables_registrar,
    IObjectLoader* _object_loader)
    :
    RoomScriptLoader(ref),
    GltfModel(materialLibrary, ref,
            [_room_name, this, _states_list, _portal_register, _object_loader](nlohmann::json& json,
            GltfDataAccessorIFace* data_accessor) {
                // build a local wrapper reference to the room provider
                return make_shared<RoomNode>(
                    json,
                    data_accessor,
                    _portal_register,
                    getScript(),
                    _room_name,
                    this,
                    _states_list,
                    _object_loader);
            }),
    room_name(_room_name),
    states_list(_states_list),
    portal_register(_portal_register),
    viewables_registrar(_viewables_registrar)
{
    instance = make_unique<GltfInstance>(getInstanceParameters());
    // collect portals list and associated index
    for (auto i =0; i < nodeTable.size(); i++) {
        auto room_node = dynamic_cast<RoomNode*>(nodeTable[i].get());
        room_node->setInstance(instance->getNode(i));
    }
    // Get specific data, if any. Needs to reload JSON
    auto raw_json = ref.readStringContent();
    auto file_json = json::parse(raw_json.c_str());
    jsonExecuteIfElement(file_json, "extras", [this](nlohmann::json& extras) {
        console->info("Found room extras");
        jsonExecuteAllIfElement(extras, "animations", [this](nlohmann::json& child, int node_index) {
            console->info("Load room animation: {}", node_index);
            room_animations.push_back(make_unique<RoomAnimation>(child, this, instance.get(), states_list));
        });
    });

    console->info("Loaded room: {}", ref.getPath());
}

void Room::updateRoom(float delta_time)
{
    glm::mat4 mat_id(1.0);
    applyDefaultTransform(instance.get(), mat_id);
    for (auto& animation_room: room_animations) {
        animation_room->animationAdvance(delta_time);
    }
}

void Room::draw(DrawContext& draw_context)
{
    //console->info("Room draw: {} - level={}", room_name, draw_context.recurse_level);
    setActiveFbo(&draw_context.fbo);
    setViewMatrix(draw_context.pov.getViewMatrix(), draw_context.pov.position);
    GltfModel::draw(instance.get(), &draw_context);
}

void Room::draw(PointOfView pov)
{
    struct DrawContext drawContext {
        pov,
        portal_register, // TODO
        0,
        FboIndex{0,0}
    };
    setViewMatrix(pov.getViewMatrix(), pov.position);
    GltfModel::draw(instance.get(), &drawContext);

    // Draw included objects
    if (viewables_registrar) {
        auto objects = viewables_registrar->getViewables(room_name);
        for (auto& obj : objects) {
            obj->outputObject();
        }
    }
}

void Room::draw(GltfInstance* instance, int index, void* context)
{
    //console->debug("room draw index {}", index);
    struct DrawContext* draw_context = (struct DrawContext*)(context);
    setClippingEquations(draw_context->clipping.getEquations());
    GltfModel::draw(instance, index, context);
    auto myNode = dynamic_cast<RoomNode*>(nodeTable[index].get());
    auto instanceNode = instance->getNode(index);
    myNode->draw(instanceNode, *draw_context);
}

bool Room::getDestinationPov(
            const PointOfView& origin,
            const PointOfView& destination,
            glm::vec3& changePoint,
            float& _distance,
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
        glm::vec3 changePointLocal;
        float distance = _distance;
        std::string dstRoom;
        RoomNode *dstRoomNode;
        GltfNodeInstanceIface *dstNodeInstance;
        bool crossed = roomNode->checkPortalCrossing(
            localOrigin.position, localDestination.position,
            gate, changePointLocal, distance,
            dstRoom, dstRoomNode, dstNodeInstance
            );
        if (crossed) {
            changePoint = roomNodeInstance->getInvertedNodeMatrix() * positionToVec4(changePointLocal);

            console->debug("Crossed portal at change point local:\n{}\ndistance={}\nportal={}/{}==>{}\nwas from local:{}\nwas going to local:\n",
                vec3_to_string(changePointLocal), distance, gate.gate, gate.connect, dstRoom,
                vec3_to_string(localOrigin.position), vec3_to_string(localDestination.position)
                );

            // compute target position
            newPovDestination = localDestination.changeCoordinateSystem(
                dstRoom,
                dstNodeInstance->getNodeMatrix());
            auto localChangePointDestination(localDestination);
            localChangePointDestination.position = changePointLocal;
            newPovChangePoint = localChangePointDestination.changeCoordinateSystem(
                dstRoom,
                dstNodeInstance->getNodeMatrix());
            _distance = distance;
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

        if (crossed && distanceWall < _distance) {
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

GravityInformation Room::getGravity(glm::vec3 position, glm::vec3 speed, float weight, float radius, float total_time)
{
    GravityInformation result;
    std::list<GravityInformation> child_gravity_list;
    for (auto root : root_nodes) {
        GravityInformation root_gravity;
        bool root_has_gravity = Room::_getGravity(root, position, speed, weight, radius, total_time, root_gravity);
        if (root_has_gravity)
            child_gravity_list.push_back(root_gravity);
    }
    return GravityInformation(child_gravity_list);
}

bool Room::_getGravity(unsigned int instance_index, glm::vec3 position, glm::vec3 speed, float weight, float radius, float total_time, GravityInformation& result)
{
    // compute for each child node and if set, bail out
    RoomNode* roomNode = dynamic_cast<RoomNode*>(nodeTable[instance_index].get());
    std::list<GravityInformation> child_gravity_list;
    for (auto child_node: roomNode->children) {
        GravityInformation child_gravity;
        _getGravity(child_node, position, speed, weight, radius, total_time, child_gravity);
        child_gravity_list.push_back(child_gravity);
    }
    auto instanceNode = instance->getNode(instance_index);
    auto localPosition = instanceNode->getInvertedNodeMatrix() *  positionToVec4(position);
    auto localSpeed = instanceNode->getInvertedNodeMatrix() * vectorToVec4(speed);
    roomNode->getGravity(localPosition, localSpeed, weight, radius, total_time, result);
    result.gravity = instanceNode->getNodeMatrix() * vectorToVec4(result.gravity);
    result.up = instanceNode->getNodeMatrix() * vectorToVec4(result.up);
    child_gravity_list.push_back(result);
    result = GravityInformation(child_gravity_list);
    return true;
}

void Room::applyTrigger(
            const glm::vec3& previous_position,
            const glm::vec3& next_position,
            const STRUCTURE_OBJECT_TYPE object_type,
            const bool activated) const
{
    // compute for each child node
    auto vec4_previous = positionToVec4(previous_position);
    auto vec4_next = positionToVec4(next_position);
    for (auto i = 0; i < nodeTable.size(); i++) {
        RoomNode* roomNode = dynamic_cast<RoomNode*>(nodeTable[i].get());
        GltfNodeInstanceIface* roomNodeInstance = dynamic_cast<GltfNodeInstanceIface*>(instance->getNode(i));

        auto local_previous = roomNodeInstance->getInvertedNodeMatrix() * vec4_previous;
        auto local_next = roomNodeInstance->getInvertedNodeMatrix() * vec4_next;

        roomNode->applyTrigger(local_previous, local_next, object_type, activated);
    }
}
