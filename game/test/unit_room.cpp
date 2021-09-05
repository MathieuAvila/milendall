#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "room.hxx"
#include "gltf_material_accessor_library_iface.hxx"

#include "glmock.hpp"

#include "common.hxx"

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
