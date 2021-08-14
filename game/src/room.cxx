
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

static auto console = spdlog::stdout_color_mt("room");

struct DrawContext {
    PointOfView pov;
    /*glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    Room* room;*/
    RoomResolver* room_resolver;
    int recurse_level;
    FboIndex fbo;
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
    // compute list of triangles
    std::vector<unsigned short> triangles;
    for (auto& f: face.getFaces()) {
        for (auto i=0; i < f.indices.size()-2; i++) {
            // for "in"
            triangles.push_back(f.indices[0].index);
            triangles.push_back(f.indices[i+1].index);
            triangles.push_back(f.indices[i+2].index);
            // for "out"
            triangles.push_back(f.indices[0].index);
            triangles.push_back(f.indices[i+2].index);
            triangles.push_back(f.indices[i+1].index);
        }
    }
    auto& p = points.get()->getPoints();
    portal_triangles = make_unique<TrianglesBufferInfo>(
        std::span<glm::vec3>(const_cast<glm::vec3*>(&p[0]), p.size()),
        std::span<unsigned short>(&triangles[0], triangles.size())
        );
    console->info("Portal from {} connecting {} and {}, is IN={}", room_name, connect[0], connect[1], in);
}

RoomNode::FaceHard::FaceHard(
    std::shared_ptr<PointsBlock> points,
    std::unique_ptr<GltfDataAccessorIFace::DataBlock> accessor,
    nlohmann::json& json) : face(points, move(accessor))
{
    console->info("Hard walls");
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
            if (type == "hard") {
                walls.push_back(FaceHard(points, move(faces_data), data));
            }
        });
    }
}

bool RoomNode::checkDrawGate(
    GltfNodeInstanceIface * currentNodeInstance,
    const DrawContext& currentDrawContext,
    const FacePortal& portal,
    const FaceList::Face& face,
    DrawContext& newDrawContext) const
{
    float factor = portal.in ? 1.0 : -1.0;

    // compute vectors in local referential
    PointOfView localOriginDC = currentDrawContext.pov.changeCoordinateSystem(
        currentDrawContext.pov.room,
        currentNodeInstance->getInvertedNodeMatrix());
/*
    console->info("org global pos {}", vec3_to_string(currentDrawContext.position));
    console->info("org global dir {}", vec3_to_string(currentDrawContext.direction));
    console->info("org global up  {}", vec3_to_string(currentDrawContext.up));
    console->info("org local  pos {}", vec3_to_string(localOriginDC.position));
    console->info("org local  dir {}", vec3_to_string(localOriginDC.direction));
    console->info("org local  up  {}", vec3_to_string(localOriginDC.up));
*/
    // compute position side and exclude if we're not on the right side
    auto side = pointPlaneProjection(face.plane, localOriginDC.position) * factor;
    if (side < 0.0)
        return false;

    // TODO: use frustum clipping instead

    // retrieve target room, and node_room objects for gate
    auto target_room_name = portal.in ? portal.connect[1] : portal.connect[0];
    auto target_room = currentDrawContext.room_resolver->getRoom(target_room_name);
    if (target_room == nullptr) {
        throw LevelException("Unable to get room:" + target_room_name);
    }
    auto [target_room_node, target_room_node_instance] = target_room->getGateNode(GateIdentifier{portal.gate, !portal.in});
    if (target_room_node == nullptr) {
        throw LevelException("Unable to get room node for gate:" + portal.gate + " with in/out: " + to_string(!portal.in));
    }
    if (target_room_node_instance == nullptr) {
        throw LevelException("Unable to get room node instance for gate:" + portal.gate + " with in/out: " + to_string(!portal.in));
    }
    // compute target
    newDrawContext = currentDrawContext;
    auto& matTarget = target_room_node_instance->getNodeMatrix();
    newDrawContext.pov = localOriginDC.changeCoordinateSystem(
        target_room_name,
        target_room_node_instance->getNodeMatrix());

    //newDrawContext.position = matTarget * positionToVec4(localOriginDC.position);
    //newDrawContext.direction = target_room_node_instance->getNodeMatrix() * vectorToVec4(localOriginDC.direction);
    //newDrawContext.up = target_room_node_instance->getNodeMatrix() * vectorToVec4(localOriginDC.up);
    //newDrawContext.room = target_room;

    return true;
}

void RoomNode::draw(GltfNodeInstanceIface * nodeInstance, DrawContext& roomContext)
{
    for (auto& portal: portals) {
        for (const auto& face : portal.face.getFaces()) {
            DrawContext newDrawContext;
            console->debug("check gate {} from room {} ( {} / {})",
                portal.gate,
                portal.in ? portal.connect[0]: portal.connect[1],
                portal.connect[0], portal.connect[1]);
            bool valid = checkDrawGate(nodeInstance, roomContext, portal, face, newDrawContext);
            console->debug(" ==> {}", valid);
            newDrawContext.recurse_level = roomContext.recurse_level + 1;
            if (valid) {
                // Get a FBO to draw to
                int result = getValidFbo(&newDrawContext.fbo);
                if (result) {
                    console->warn("No available FBO, must have gone too far. Sad.");
                    return;
                }
                //console->info("FBO = " + to_string(newDrawContext.fbo.fboIndex));

                // draw room
                newDrawContext.room_resolver->getRoom(newDrawContext.pov.room)->draw(newDrawContext);

                // switch to original FBO and context
                setActiveFbo(&roomContext.fbo);
                setViewComponents(roomContext.pov.position, roomContext.pov.direction, roomContext.pov.up);
                setMeshMatrix(nodeInstance->getNodeMatrix());

                // switch to portal program and draw portal
                activatePortalDrawingProgram();
                glActiveTexture(GL_TEXTURE0);
	            glBindTexture(GL_TEXTURE_2D, newDrawContext.fbo.textureIndex);
                portal.portal_triangles->draw();

                // switch back to main program
                activateDefaultDrawingProgram();

            }
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

void Room::draw(DrawContext& draw_context)
{
    //console->info("Room draw: {} - level={}", room_name, draw_context.recurse_level);
    setActiveFbo(&draw_context.fbo);
    setViewComponents(draw_context.pov.position, draw_context.pov.direction, draw_context.pov.up);
    GltfModel::draw(instance.get(), &draw_context);
}

void Room::draw(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    struct DrawContext drawContext {
        PointOfView{
        position, direction, up,
        this->room_name},
        room_resolver,
        0,
        FboIndex{0,0}
    };
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