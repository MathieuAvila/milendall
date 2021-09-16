#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "room.hxx"
#include "gltf_material_accessor_library_iface.hxx"

#include "glmock.hpp"

#include "common.hxx"
#include "helper_math.hxx"

using ::testing::_;
using ::testing::InSequence;
using namespace std;

static auto console = spdlog::stdout_color_mt("ut_room");


std::unique_ptr<Room> loadRoom(std::string roomPath)
{
    auto materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    auto roomPathRef = fl.getRoot().getSubPath(roomPath);
    auto room = make_unique<Room>("room1", materialLibrary, roomPathRef);
    auto room_ptr = room.get();
    EXPECT_NE( room_ptr, nullptr );
    room->applyTransform();
    return room;
}

TEST(Room, LoadLevel2Rooms1Gate_Room1) {

    InSequence s;
    GLMock mock;

    auto room = loadRoom("/2_rooms_1_gate/room1/room.gltf");


    EXPECT_EQ(room.get()->nodeTable.size(), 4);
    auto & table = room.get()->nodeTable;
    // next tests are for sanity checks, just to assure tests are done on the right objects.
    EXPECT_EQ(table[0].get()->name, "");
    EXPECT_EQ(table[1].get()->name, "parent");
    EXPECT_EQ(table[2].get()->name, "r1r2");
    EXPECT_EQ(table[3].get()->name, "r1r2_impl");

    // checking portal
    GltfNode* node_portal = table[3].get();
    RoomNode* nodePortal = dynamic_cast<RoomNode*>(node_portal);
    EXPECT_EQ(nodePortal->portals.size(), 1);
    EXPECT_EQ(nodePortal->portals.front().connect[0], "room1");
    EXPECT_EQ(nodePortal->portals.front().connect[1], "room2");
    EXPECT_EQ(nodePortal->portals.front().face->getFaces().size(), 1); // should contain 1 face

    // checking one that has no portals
    GltfNode* node_parent = table[1].get();
    RoomNode* nodeParent = dynamic_cast<RoomNode*>(node_parent);
    EXPECT_EQ(nodeParent->portals.size(), 0);

}

TEST(Room, GateLoading__LoadLevel3Rooms3Gate_Room1) {

    InSequence s;
    GLMock mock;

    auto room = loadRoom("/3_rooms_3_gates/room1/room.gltf");

    auto portal_list = room->getGateNameList();

    for (auto e: portal_list)
        console->info("{} {}", e.from, e.gate);

    EXPECT_EQ( portal_list, (std::set<GateIdentifier>{
        GateIdentifier{"r2r1",false},
        GateIdentifier{"r1r2",true},
        GateIdentifier{"r3r1",false}}) );

    auto [room_node, instance] = room->getGateNode(GateIdentifier{"r2r1",false});
    EXPECT_EQ( room_node->name, "r2r1_impl" );
    //EXPECT_EQ( instance->, "" ); // don't know how to check this
}


TEST(Room, isWallReached_0_simple_NOT_reached) {

    // don't touch anything

    InSequence s;
    GLMock mock;

    auto room = loadRoom("/3_rooms_3_gates/room1/room.gltf");

    glm::vec3 origin(3.0f, 2.0f, 2.5f);
    glm::vec3 destination(3.0f, 1.0f, 2.5f);
    float radius = 0.5f;

    glm::vec3 hitPoint;
    glm::vec3 normal;
    float distance = glm::length(origin - destination);
    FaceHard *face;

    bool reached = room->isWallReached(origin, destination,radius,hitPoint, normal, distance, face);

    //console->info("Check wall org {}", vec3_to_string(origin));
    //console->info("Check wall dst {}", vec3_to_string(destination));
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));

    EXPECT_FALSE(reached);
}

TEST(Room, isWallReached_1_simple_reached) {

    // just hit the floor on 1 point

    InSequence s;
    GLMock mock;

    auto room = loadRoom("/3_rooms_3_gates/room1/room.gltf");

    glm::vec3 origin(3.0f, 2.0f, 2.5f);
    glm::vec3 destination(3.0f, -1.0f, 2.5f);
    float radius = 1.0f;

    glm::vec3 hitPoint;
    glm::vec3 normal;
    float distance = glm::length(origin - destination);
    FaceHard *face;

    bool reached = room->isWallReached(origin, destination,radius,hitPoint, normal, distance, face);

    //console->info("Check wall org {}", vec3_to_string(origin));
    //console->info("Check wall dst {}", vec3_to_string(destination));
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));

    EXPECT_TRUE(reached);
    ASSERT_FLOAT_EQ(distance, 1.0f);
    EXPECT_EQ(normal, glm::vec3(0.0f, 1.0f, 0.0f)); // up
    EXPECT_EQ(hitPoint, glm::vec3(3.0f, 1.0f, 2.5f));
}


TEST(Room, isWallReached_2_simple_CORNER_reached) {

    // corner, there are 3 possible walls

    InSequence s;
    GLMock mock;

    auto room = loadRoom("/3_rooms_3_gates/room1/room.gltf");

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
    reached = room->isWallReached(origin, destination,radius,hitPoint, normal, distance, face);
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));
    EXPECT_TRUE(reached);
    ASSERT_FLOAT_EQ(distance, 1.7320509f);
    EXPECT_TRUE(glm::length(normal - glm::vec3(0.0f, 1.0f, 0.0f)) < 0.1f); // up
    EXPECT_TRUE(glm::length(hitPoint - glm::vec3(1.1f , 1.0f , 1.1f)) < 0.1f);

    // 2: hit wall 1
    origin = glm::vec3(2.0f, 2.1f, 2.1f);
    destination = glm::vec3(-2.0f, -1.9f, -1.9f);
    distance = glm::length(origin - destination);
    reached = room->isWallReached(origin, destination,radius,hitPoint, normal, distance, face);
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
    reached = room->isWallReached(origin, destination,radius,hitPoint, normal, distance, face);
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));
    EXPECT_TRUE(reached);
    ASSERT_FLOAT_EQ(distance, 1.7320509f);
    EXPECT_TRUE(glm::length(normal - glm::vec3(0.0 , 0.0 , 1.0f)) < 0.1f); // up
    EXPECT_TRUE(glm::length(hitPoint - glm::vec3( 1.1f , 1.1f , 1.0f)) < 0.1f);
}


TEST(Room, isWallReached_3_sub_object) {

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
    glm::vec3 origin(6.5f, 2.0f, 2.0f);
    glm::vec3 destination(6.5f, 2.0f, -2.0f);
    distance = glm::length(origin - destination);
    reached = room->isWallReached(origin, destination,radius,hitPoint, normal, distance, face);
    console->info("Check wall distance {}", distance);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("Check wall hitPoint {}", vec3_to_string(hitPoint));
    EXPECT_TRUE(reached);
    ASSERT_FLOAT_EQ(distance, 0.8f);
    EXPECT_TRUE(glm::length(normal - glm::vec3(0.0f , 0.0f , 1.0f)) < 0.1f); // it must be in global space
    EXPECT_TRUE(glm::length(hitPoint - glm::vec3(6.5f, 2.0f, 1.2)) < 0.1f);

}
