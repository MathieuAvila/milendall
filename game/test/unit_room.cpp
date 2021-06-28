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

TEST(Room, LoadLevel2Rooms1Gate_Room1) {

    InSequence s;
    GLMock mock;

    auto materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    auto roomPath = fl.getRoot().getSubPath("/2_rooms_1_gate/room1/room.gltf");
    auto room = make_unique<Room>(materialLibrary, roomPath);
    EXPECT_TRUE( room.get() != nullptr );
}


