#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <map>
#include <utility>

#include "room.hxx"
#include "gltf_material_accessor_library_iface.hxx"
#include "states_list.hxx"
#include "impl_room_node_portal_register.hxx"

#include "glmock.hpp"

#include "common.hxx"
#include "helper_math.hxx"

#include "iface_object_loader.hxx"

using ::testing::_;
using ::testing::InSequence;
using namespace std;

static auto console = getConsole("ut_room");

class RoomTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        spdlog::get("unit_states_list")->set_level(spdlog::level::debug);
        spdlog::get("states_list")->set_level(spdlog::level::debug);
    }
};

struct PairRoomStates
{
    unique_ptr<Room> room;
    unique_ptr<StatesList> states;
    PairRoomStates(unique_ptr<Room> &_room, unique_ptr<StatesList> &_states)
    {
        room.swap(_room);
        states.swap(_states);
    };
};

PairRoomStates loadRoomWithStates(std::string roomPath)
{
    auto materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    auto roomPathRef = fl.getRoot().getSubPath(roomPath);
    auto states_list = make_unique<StatesList>();
    auto room = make_unique<Room>("room1", materialLibrary, roomPathRef, nullptr, states_list.get());
    auto room_ptr = room.get();
    EXPECT_NE(room_ptr, nullptr);
    room->updateRoom(0.0f);
    return PairRoomStates(room, states_list);
}

std::unique_ptr<Room> loadRoom(std::string roomPath)
{
    PairRoomStates all = loadRoomWithStates(roomPath);
    std::unique_ptr<Room> result;
    result.swap(all.room);
    return result;
}

std::tuple<
    std::shared_ptr<StatesList>,
    std::shared_ptr<Room>,
    std::shared_ptr<ImplRoomNodePortalRegister>>
loadRoomFull(
    std::string roomPath,
    ViewablesRegistrar *viewables_registrar = nullptr,
    IObjectLoader *object_loader = nullptr)
{
    auto materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    auto roomPathRef = fl.getRoot().getSubPath(roomPath);
    auto states_list = make_shared<StatesList>();
    auto portal_register = make_shared<ImplRoomNodePortalRegister>();
    auto room = make_shared<Room>(
        "room1",
        materialLibrary,
        roomPathRef,
        portal_register.get(),
        states_list.get(),
        viewables_registrar,
        object_loader);
    auto room_ptr = room.get();
    EXPECT_NE(room_ptr, nullptr);
    room->updateRoom(0.0f);
    return std::make_tuple(states_list, room, portal_register);
}

TEST_F(RoomTest, LoadLevel2Rooms1Gate)
{

    InSequence s;
    GLMock mock;

    auto [states, room, portal_register] = loadRoomFull("/2_rooms_1_gate/room1/room.gltf");

    EXPECT_EQ(room.get()->nodeTable.size(), 4);
    auto &table = room.get()->nodeTable;
    // next tests are for sanity checks, just to assure tests are done on the right objects.
    EXPECT_EQ(table[0].get()->name, "#0");
    EXPECT_EQ(table[1].get()->name, "b0_parent");
    EXPECT_EQ(table[2].get()->name, "b0_root_door0");
    EXPECT_EQ(table[3].get()->name, "door0_door0");

    // checking portal
    GltfNode *node_portal = table[3].get();
    RoomNode *nodePortal = dynamic_cast<RoomNode *>(node_portal);
    EXPECT_EQ(nodePortal->portals.size(), 1);
    EXPECT_EQ(nodePortal->portals.front().gate, GateIdentifier("my_door", "A"));
    EXPECT_EQ(nodePortal->portals.front().face->getFaces().size(), 1); // should contain 1 face

    // checking one that has no portals
    GltfNode *node_parent = table[1].get();
    RoomNode *nodeParent = dynamic_cast<RoomNode *>(node_parent);
    EXPECT_EQ(nodeParent->portals.size(), 0);

    // checking what was registered
    EXPECT_EQ(node_portal, portal_register->getPortal(GateIdentifier("my_door", "A")));
}

TEST_F(RoomTest, GateLoading__LoadLevel3Rooms3Gate)
{

    InSequence s;
    GLMock mock;

    auto [states, room, portal_register] = loadRoomFull("/3_rooms_3_gates_room1/room.gltf");

    auto &table = room.get()->nodeTable;
    for (auto n : table)
    {
        GltfNode *node = table[2].get();
        console->info("Node {}={}", n->name, (void *)node);
    }

    EXPECT_EQ("r1r2_impl", portal_register->getPortal(GateIdentifier("r1r2", "B"))->name);
    EXPECT_EQ("r2r1_impl", portal_register->getPortal(GateIdentifier("r2r1", "A"))->name);
    EXPECT_EQ("r3r1_impl", portal_register->getPortal(GateIdentifier("r3r1", "A"))->name);
}

TEST_F(RoomTest, isWallReached_0_simple_NOT_reached)
{

    // don't touch anything

    InSequence s;
    GLMock mock;

    auto room = loadRoom("/3_rooms_3_gates_room1/room.gltf");

    glm::vec3 origin(3.0f, 2.0f, 2.5f);
    glm::vec3 destination(3.0f, 1.0f, 2.5f);
    float radius = 0.5f;

    glm::vec3 hitPoint;
    glm::vec3 normal;
    float distance = glm::length(origin - destination);
    FaceHard *face;

    bool reached = room->isWallReached(origin, destination, radius, hitPoint, normal, distance, face);

    // console->info("Check wall org {}", vec3_to_string(origin));
    // console->info("Check wall dst {}", vec3_to_string(destination));
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));

    EXPECT_FALSE(reached);
}

TEST_F(RoomTest, isWallReached_1_simple_reached)
{

    // just hit the floor on 1 point

    InSequence s;
    GLMock mock;

    auto room = loadRoom("/3_rooms_3_gates_room1/room.gltf");

    glm::vec3 origin(3.0f, 2.0f, 2.5f);
    glm::vec3 destination(3.0f, -1.0f, 2.5f);
    float radius = 1.0f;

    glm::vec3 hitPoint;
    glm::vec3 normal;
    float distance = glm::length(origin - destination);
    FaceHard *face;

    bool reached = room->isWallReached(origin, destination, radius, hitPoint, normal, distance, face);

    // console->info("Check wall org {}", vec3_to_string(origin));
    // console->info("Check wall dst {}", vec3_to_string(destination));
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));

    EXPECT_TRUE(reached);
    ASSERT_FLOAT_EQ(distance, 1.0f);
    EXPECT_EQ(normal, glm::vec3(0.0f, 1.0f, 0.0f)); // up
    EXPECT_EQ(hitPoint, glm::vec3(3.0f, 1.0f, 2.5f));
}

TEST_F(RoomTest, isWallReached_2_simple_CORNER_reached)
{

    // corner, there are 3 possible walls

    InSequence s;
    GLMock mock;

    auto room = loadRoom("/3_rooms_3_gates_room1/room.gltf");

    float radius = 1.0f;
    glm::vec3 hitPoint;
    glm::vec3 normal;
    float distance;
    FaceHard *face;
    bool reached;

    // 1: hit ground
    glm::vec3 origin(2.1f, 2.0f, 2.1f);
    glm::vec3 destination(-1.9f, -2.0f, -1.9f);
    distance = glm::length(origin - destination);
    reached = room->isWallReached(origin, destination, radius, hitPoint, normal, distance, face);
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));
    EXPECT_TRUE(reached);
    ASSERT_FLOAT_EQ(distance, 1.7320509f);
    EXPECT_TRUE(glm::length(normal - glm::vec3(0.0f, 1.0f, 0.0f)) < 0.1f); // up
    EXPECT_TRUE(glm::length(hitPoint - glm::vec3(1.1f, 1.0f, 1.1f)) < 0.1f);

    // 2: hit wall 1
    origin = glm::vec3(2.0f, 2.1f, 2.1f);
    destination = glm::vec3(-2.0f, -1.9f, -1.9f);
    distance = glm::length(origin - destination);
    reached = room->isWallReached(origin, destination, radius, hitPoint, normal, distance, face);
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));
    EXPECT_TRUE(reached);
    EXPECT_EQ(distance, 1.7320509f);
    EXPECT_TRUE(glm::length(normal - glm::vec3(1.0f, 0.0f, 0.0f)) < 0.1f); // up
    EXPECT_TRUE(glm::length(hitPoint - glm::vec3(1.0f, 1.1f, 1.1f)) < 0.1f);

    // 3: hit wall 2
    origin = glm::vec3(2.1f, 2.1f, 2.0f);
    destination = glm::vec3(-1.9f, -1.9f, -2.0f);
    distance = glm::length(origin - destination);
    reached = room->isWallReached(origin, destination, radius, hitPoint, normal, distance, face);
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));
    EXPECT_TRUE(reached);
    ASSERT_FLOAT_EQ(distance, 1.7320509f);
    EXPECT_TRUE(glm::length(normal - glm::vec3(0.0, 0.0, 1.0f)) < 0.1f); // up
    EXPECT_TRUE(glm::length(hitPoint - glm::vec3(1.1f, 1.1f, 1.0f)) < 0.1f);
}

TEST_F(RoomTest, isWallReached_3_sub_object)
{

    // check a cross with an object that has a non-ID matrix
    InSequence s;
    GLMock mock;

    auto room = loadRoom("/2_rooms_1_gate/room1/room.gltf");

    float radius = 1.0f;
    glm::vec3 hitPoint;
    glm::vec3 normal;
    float distance;
    FaceHard *face;
    bool reached;

    // will cross a door wall
    glm::vec3 origin(2.5f, 1.5f, 2.0f);
    glm::vec3 destination(2.5f, 1.5f, -2.0f);
    distance = glm::length(origin - destination);
    reached = room->isWallReached(origin, destination, radius, hitPoint, normal, distance, face);
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));
    EXPECT_TRUE(reached);
    ASSERT_FLOAT_EQ(distance, 1.0f);
    EXPECT_TRUE(glm::length(normal - glm::vec3(0.0f, 0.0f, 1.0f)) < 0.1f); // it must be in global space
    EXPECT_TRUE(glm::length(hitPoint - glm::vec3(2.5f, 1.5f, 1.0f)) < 0.1f);
}

TEST_F(RoomTest, gravity_0__default)
{
    // testing what happens when no node defines gravity
    InSequence s;
    GLMock mock;

    auto room = loadRoom("/2_rooms_1_gate/room1/room.gltf");

    GravityInformation gravity = room->getGravity(glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, 1.0f, 1.0f);
    GravityInformation default_gravity;

    EXPECT_EQ(gravity, default_gravity);
}

TEST_F(RoomTest, gravity_1__1_node)
{
    // testing what happens when 1 single root node defines gravity
    InSequence s;
    GLMock mock;
    auto room = loadRoom("/room_gravity/1_node/room.gltf");

    GravityInformation gravity = room->getGravity(glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, 1.0f, 1.0f);
    GravityInformation applied_gravity(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0f), 1000.0f, 1.0f);

    console->info("gravity {}", vec3_to_string(applied_gravity.gravity));
    console->info("up {}", vec3_to_string(applied_gravity.up));
    console->info("validity {}", applied_gravity.validity);

    console->info("gravity {}", vec3_to_string(gravity.gravity));
    console->info("up {}", vec3_to_string(gravity.up));
    console->info("validity {}", gravity.validity);

    EXPECT_EQ(gravity, applied_gravity);
}

TEST_F(RoomTest, gravity_2__1_child_node_no_matrix)
{
    // testing what happens when 1 single child node defines gravity, without translation matrix
    InSequence s;
    GLMock mock;
    auto room = loadRoom("/room_gravity/1_child_node_no_matrix/room.gltf");

    GravityInformation gravity = room->getGravity(glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, 1.0f, 1.0f);
    GravityInformation applied_gravity(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0f), 1000.0f, 1.0f);

    console->info("gravity {}", vec3_to_string(applied_gravity.gravity));
    console->info("up {}", vec3_to_string(applied_gravity.up));
    console->info("validity {}", applied_gravity.validity);

    console->info("gravity {}", vec3_to_string(gravity.gravity));
    console->info("up {}", vec3_to_string(gravity.up));
    console->info("validity {}", gravity.validity);

    EXPECT_EQ(gravity, applied_gravity);
}

TEST_F(RoomTest, gravity_3__1_child_node_matrix)
{
    // testing what happens when 1 single child node defines gravity, with a translation matrix
    // matrix is an inversion on Z. Local gravity/up is UP on Z, so it becomes DOWN on Z
    InSequence s;
    GLMock mock;
    auto room = loadRoom("/room_gravity/1_child_node_matrix/room.gltf");

    GravityInformation gravity = room->getGravity(glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, 1.0f, 1.0f);
    GravityInformation applied_gravity(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0f), 1000.0f, 1.0f);

    console->info("gravity {}", vec3_to_string(applied_gravity.gravity));
    console->info("up {}", vec3_to_string(applied_gravity.up));
    console->info("validity {}", applied_gravity.validity);

    console->info("gravity {}", vec3_to_string(gravity.gravity));
    console->info("up {}", vec3_to_string(gravity.up));
    console->info("validity {}", gravity.validity);

    EXPECT_EQ(gravity, applied_gravity);
}

TEST_F(RoomTest, gravity_4__2_child_node)
{
    // testing what happens when 2 child nodes defines gravity, must be combined
    InSequence s;
    GLMock mock;
    auto room = loadRoom("/room_gravity/2_child_node/room.gltf");

    GravityInformation gravity = room->getGravity(glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, 1.0f, 1.0f);
    GravityInformation applied_gravity(glm::vec3(1.5, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0f), 1000.0f, 1.0f);

    console->info("gravity {}", vec3_to_string(applied_gravity.gravity));
    console->info("up {}", vec3_to_string(applied_gravity.up));
    console->info("validity {}", applied_gravity.validity);

    console->info("gravity {}", vec3_to_string(gravity.gravity));
    console->info("up {}", vec3_to_string(gravity.up));
    console->info("validity {}", gravity.validity);

    EXPECT_EQ(gravity, applied_gravity);
}

TEST_F(RoomTest, gravity_5__2_child_node_1_root)
{
    // testing what happens when 1 root node AND 2 child nodes defines gravity, must keep children
    InSequence s;
    GLMock mock;
    auto room = loadRoom("/room_gravity/2_child_node_1_root/room.gltf");

    GravityInformation gravity = room->getGravity(glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, 1.0f, 1.0f);
    GravityInformation applied_gravity(glm::vec3(8.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0f), 1000.0f, 1.0f);

    console->info("gravity {}", vec3_to_string(applied_gravity.gravity));
    console->info("up {}", vec3_to_string(applied_gravity.up));
    console->info("validity {}", applied_gravity.validity);

    console->info("gravity {}", vec3_to_string(gravity.gravity));
    console->info("up {}", vec3_to_string(gravity.up));
    console->info("validity {}", gravity.validity);

    EXPECT_EQ(gravity, applied_gravity);
}

TEST_F(RoomTest, gravity_6__load_script)
{
    // testing global script loading
    InSequence s;
    GLMock mock;
    auto room = loadRoom("/room_gravity/load_script/room.gltf");
    GravityInformation gravity = room->getGravity(glm::vec3(0.0f), glm::vec3(0.0f), 1.0f, 2.0f, 3.0f);
    console->info("gravity {}", vec3_to_string(gravity.gravity));
    console->info("up {}", vec3_to_string(gravity.up));
    console->info("validity {}", gravity.validity);

    // must apply function for r1r2
    ASSERT_EQ(gravity.up, glm::vec3(0.6f, 0.8f, 0.8f));
    ASSERT_EQ(gravity.validity, 1.0f);
}

TEST_F(RoomTest, animations)
{
    // testing loading animations and applying
    InSequence s;
    GLMock mock;
    auto [states, room, portal_register] = loadRoomFull("/room_2_animations/room.gltf");

    auto &anim = room.get()->room_animations;
    const auto &first = anim.begin()->get();
    const auto &second = std::next(anim.begin(), 1)->get();

    ASSERT_EQ(anim.size(), 2);
    ASSERT_EQ(first->animation, "open_door1");
    ASSERT_EQ(second->animation, "open_door2");

    ASSERT_EQ(first->time_current, 0.0f);
    ASSERT_EQ(second->time_current, 0.0f);

    room->updateRoom(0.1f);

    ASSERT_EQ(first->time_current, 0.0f);
    ASSERT_EQ(second->time_current, 0.0f);

    states->setState("my_door_event_1", true);
    room->updateRoom(0.1f);

    ASSERT_EQ(first->time_current, 0.1f);
    ASSERT_EQ(second->time_current, 0.0f);
}

TEST_F(RoomTest, objects_with_no_loader)
{
    // testing loading animations and applying
    InSequence s;
    GLMock mock;
    auto [states, room, portal_register] = loadRoomFull("/1_room_4_objects_different_meshes/room.gltf");
}

class TestObjectLoader : public IObjectLoader
{
public:
    typedef struct obj_s
    {
        std::string room_name;
        std::string mesh_name;
        std::string id;
        bool operator==(const struct obj_s& b) const
        {
            return
                room_name == b.room_name &&
                mesh_name == b.mesh_name &&
                id == b.id;
        };
    } obj;

    std::vector<obj> objs;

    virtual void setReferences(
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider,
            ViewablesRegistrar* _viewables_registrar) override
    {

    }

    virtual void loadObject(std::string room_name, std::string mesh_name, nlohmann::json &root) override
    {
        obj o{room_name, mesh_name, root["type"].get<std::string>()};
        console->info("object {} - {} - {}", o.room_name, o.mesh_name, o.id);
        objs.push_back(o);
    };
    virtual ~TestObjectLoader(){};
};

TEST_F(RoomTest, objects_with_loader)
{
    // testing loading animations and applying
    InSequence s;
    GLMock mock;
    auto object_loader = make_unique<TestObjectLoader>();
    auto [states, room, portal_register] = loadRoomFull(
        "/1_room_4_objects_different_meshes/room.gltf",
        nullptr,
        object_loader.get());
    ASSERT_EQ(object_loader->objs.size(), 4);
    ASSERT_EQ(object_loader->objs[0], (TestObjectLoader::obj{ "room1", "#0", "object_0"}));
    ASSERT_EQ(object_loader->objs[1], (TestObjectLoader::obj{ "room1", "#0", "object_1"}));
    ASSERT_EQ(object_loader->objs[2], (TestObjectLoader::obj{ "room1", "objects_objects", "object_2"}));
    ASSERT_EQ(object_loader->objs[3], (TestObjectLoader::obj{ "room1", "objects_objects", "object_3"}));
}