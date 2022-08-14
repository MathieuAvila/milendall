
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include <glm/gtc/matrix_transform.hpp>

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
#include "iface_room_node_portal_register.hxx"
#include "gate_id.hxx"
#include "iface_object_loader.hxx"

static auto console = getConsole("room_node");

RoomNode::FacePortal::FacePortal(
    std::shared_ptr<PointsBlock> points,
    std::unique_ptr<GltfDataAccessorIFace::DataBlock> accessor,
    nlohmann::json &json,
    const std::string &room_name)
{
    gate = GateIdentifier(
        jsonGetElementByName(json, "gate").get<string>(),
        jsonGetElementByName(json, "connect").get<string>());
    face = make_unique<FaceList>(points, move(accessor));
    // compute list of triangles
    std::vector<unsigned short> triangles;
    for (auto &f : face->getFaces())
    {
        for (auto i = 0; i < f.indices.size() - 2; i++)
        {
            // for "in"
            triangles.push_back(f.indices[0].index);
            triangles.push_back(f.indices[i + 1].index);
            triangles.push_back(f.indices[i + 2].index);
            // for "out"
            triangles.push_back(f.indices[0].index);
            triangles.push_back(f.indices[i + 2].index);
            triangles.push_back(f.indices[i + 1].index);
        }
    }
    auto &p = points.get()->getPoints();
    portal_triangles = make_unique<TrianglesBufferInfo>(
        std::span<glm::vec3>(const_cast<glm::vec3 *>(&p[0]), p.size()),
        std::span<unsigned short>(&triangles[0], triangles.size()));
    console->info("Portal {} connect {}", gate.gate, gate.connect);
}

RoomNode::RoomNode(
    nlohmann::json &json,
    GltfDataAccessorIFace *data_accessor,
    IRoomNodePortalRegister *_portal_register,
    Script *_roomScript,
    const std::string &_room_name,
    Room *_room,
    StatesList *_states_list,
    IObjectLoader* _object_loader) : GltfNode(json), room(_room), room_name(_room_name), portal_register(_portal_register)
{
    gravity = make_unique<RoomNodeGravity>(name, _roomScript);

    if (json.contains("extras"))
    {
        auto extras = json["extras"];
        if (extras.contains("points"))
        {
            auto accessor_points = jsonGetElementByName(extras, "points").get<int>();
            console->info("Found application data (extras) for RoomNode, points are {}", accessor_points);
            auto points_accessor = data_accessor->accessId(accessor_points);
            points = make_shared<PointsBlock>(move(points_accessor));

            jsonExecuteAllIfElement(extras, "phys_faces", [this, _portal_register, data_accessor](nlohmann::json &phys, int node_index)
                                    {
            auto data = jsonGetElementByName(phys, "data");
            auto type = jsonGetElementByName(data, "type").get<string>();
            auto accessor = jsonGetElementByName(phys, "accessor").get<int>();
            console->info("found phys_face:{} with accessor={}, type is {}", to_string(data), accessor, type);
            auto faces_data = data_accessor->accessId(accessor);
            if (type == "portal") {
                portals.push_back(FacePortal(points, move(faces_data), data, room_name));
                if (_portal_register)
                    _portal_register->registerPortal(portals.back().gate, this);
            }
            if (type == "hard") {
                walls.push_back(FaceHard(points, move(faces_data), data));
            } });
        }

        jsonExecuteAllIfElement(extras, "triggers", [this, _states_list](nlohmann::json &trigger, int node_index)
                                {
            console->info("found trigger :{}", node_index);
            triggers.push_back(Trigger(trigger, _states_list)); });

        jsonExecuteIfElement(extras, "gravity", [this](nlohmann::json &json_gravity)
                             { gravity->readParameters(json_gravity); });

        jsonExecuteAllIfElement(extras, "objects", [this, _object_loader](nlohmann::json& element, unsigned i) {
            if (_object_loader) {
                _object_loader->loadObject(room_name, name, element);
            } else
                console->warn("object found, but no object_loader");
        });
    }
}

bool RoomNode::checkDrawGate(
    GltfNodeInstanceIface *currentNodeInstance,
    const DrawContext &currentDrawContext,
    const FacePortal &portal,
    const FaceList::Face &face,
    DrawContext &newDrawContext,
    Room *&newRoom) const
{
    // compute vectors in local referential
    PointOfView localOriginDC = currentDrawContext.pov.changeCoordinateSystem(
        currentDrawContext.pov.room,
        currentNodeInstance->getInvertedNodeMatrix());

    // compute position side and exclude if we're not on the right side
    auto side = pointPlaneProjection(face.plane, localOriginDC.position);
    if (side <= 0.0)
        return false;

    // compute face in final space, and apply clipping
    std::vector<glm::vec3> polygon;

    // ugly: should put this elsewhere
    float FoV = 45.0f;
    auto ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.0001f, 100.0f);

    glm::mat4x4 mat =
        (ProjectionMatrix * currentDrawContext.pov.getViewMatrix() * currentNodeInstance->getNodeMatrix());
    face.getPolygon(mat, polygon);
    ClippingPlanes newClipping = currentDrawContext.clipping;
    newClipping.clipByPolygon(polygon);

    // exclude if everything is clipped out.
    if (newClipping.isVoid())
    {
        return false;
    }

    // retrieve target room, and node_room objects for gate
    auto target_connect = portal.gate.connect == "A" ? "B" : "A";
    auto target_gate = GateIdentifier(portal.gate.gate, target_connect);
    auto target_room_node = currentDrawContext.portal_provider->getPortal(target_gate);
    if (target_room_node == nullptr)
    {
        throw LevelException("Unable to get gate for :" + portal.gate.gate + " with target: " + target_connect);
    }
    auto target_room_node_instance = target_room_node->node_instance;
    if (target_room_node_instance == nullptr)
    {
        throw LevelException("Unable to get target_room_node_instance for gate:" + portal.gate.gate + " with target: " + target_connect);
    }
    if (target_room_node_instance == nullptr)
    {
        throw LevelException("Unable to get room node instance for gate:" + portal.gate.gate + " with target: " + target_connect);
    }
    // compute target
    newRoom = target_room_node->room;
    newDrawContext = currentDrawContext;
    newDrawContext.clipping = newClipping;
    newDrawContext.pov = localOriginDC.changeCoordinateSystem(
        target_room_node->room_name,
        target_room_node_instance->getNodeMatrix());

    return true;
}

void RoomNode::draw(GltfNodeInstanceIface *nodeInstance, DrawContext &roomContext)
{
    for (auto &portal : portals)
    {
        for (const auto &face : portal.face->getFaces())
        {
            DrawContext newDrawContext;
            Room *target_room;
            bool valid = checkDrawGate(nodeInstance, roomContext, portal, face, newDrawContext, target_room);
            // console->info(" ==> {}", valid);
            newDrawContext.recurse_level = roomContext.recurse_level + 1;
            if (newDrawContext.recurse_level >= 8)
            {
                valid = false;
            }

            if (valid)
            {
                // Get a FBO to draw to
                int result = getValidFbo(&newDrawContext.fbo);
                if (result)
                {
                    console->warn("No available FBO, must have gone too far. Sad.");
                    return;
                }
                // draw room
                target_room->draw(newDrawContext);

                // switch to original FBO and context
                setActiveFbo(&roomContext.fbo);
                setViewMatrix(roomContext.pov.getViewMatrix(), roomContext.pov.position);
                setMeshMatrix(nodeInstance->getNodeMatrix());

                // switch to portal program and draw portal in current context
                activatePortalDrawingProgram();
                setClippingEquations(roomContext.clipping.getEquations());

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, newDrawContext.fbo.textureIndex);
                portal.portal_triangles->draw();

                // switch back to main program
                activateDefaultDrawingProgram();
            }
        }
    }
}

bool RoomNode::checkPortalCrossing(
    const glm::vec3 &origin,
    const glm::vec3 &destination,
    GateIdentifier &gate,
    glm::vec3 &changePoint,
    float &distance,
    std::string &dstRoom,
    RoomNode *&dstRoomNode,
    GltfNodeInstanceIface *&dstNodeInstance)
{
    bool result = false;

    for (auto &portal : portals)
    {
        FaceList &pFace = *portal.face.get();
        for (auto &facePortal : pFace.getFaces())
        {
            glm::vec3 impact;
            float portalDistance;
            glm::vec3 normal;
            console->debug("Check portal {} from {} to {}",
                           gate.gate,
                           vec3_to_string(origin),
                           vec3_to_string(destination));
            bool crossed = facePortal.checkTrajectoryCross(origin, destination, impact, portalDistance, normal, false);
            if (crossed)
            {
                if ((result == false) || (portalDistance < distance))
                {
                    changePoint = impact;
                    distance = portalDistance;
                    auto target = portal.gate.connect == "A" ? "B" : "A";
                    gate = GateIdentifier{portal.gate.gate, target};
                    RoomNode *target_room_node = portal_register->getPortal(gate);
                    if (target_room_node == nullptr)
                    {
                        console->warn("No portal for gate {} {}", gate.gate, gate.connect);
                    }
                    dstRoom = target_room_node->room_name;
                    dstNodeInstance = target_room_node->node_instance;
                    result = true;
                    console->info("Portal {} was crossed, going to room {}", gate.gate, dstRoom);
                }
            }
        }
    }
    return result;
}

bool RoomNode::isWallReached(
    const glm::vec3 &origin,
    const glm::vec3 &destination,
    float radius,
    glm::vec3 &hitPoint,
    glm::vec3 &normal,
    float &_distance,
    FaceHard *&face)
{
    auto distance = _distance;
    bool result = false;

    for (auto &wall : walls)
    {
        FaceList &lFace = wall.face;
        for (auto &c_face : lFace.getFaces())
        {
            glm::vec3 wallHitPoint;
            glm::vec3 wallNormal;
            float distanceWall = distance;
            bool crossed = c_face.checkSphereTrajectoryCross(origin, destination, radius, wallHitPoint, distanceWall, wallNormal);
            // if (crossed)
            //     console->info("Check wall crossed {} distance {} distanceWall {}",
            //     crossed, distance, distanceWall);
            if (crossed && distanceWall < _distance)
            {
                normal = wallNormal;
                _distance = distanceWall;
                hitPoint = wallHitPoint;
                face = &wall;
                result = true;
                // console->info("Hit wall at: {}, distance={}", vec3_to_string(wallHitPoint), distance);
                if (_distance == 0.0f)
                    break;
            }
        }
    }
    return result;
}

bool RoomNode::getGravity(glm::vec3 position, glm::vec3 speed, float weight, float radius, float total_time, GravityInformation &_gravity)
{
    return gravity->getGravityInformation(position, speed, weight, radius, total_time, _gravity);
}

void RoomNode::applyTrigger(
    const glm::vec3 &previous_position,
    const glm::vec3 &next_position,
    const STRUCTURE_OBJECT_TYPE object_type,
    const bool activated) const
{
    for (auto &trigger : triggers)
    {
        trigger.applyTrigger(previous_position, next_position, 0.0f, activated);
    }
}

void RoomNode::setInstance(GltfNodeInstanceIface *_node_instance)
{
    node_instance = _node_instance;
}
