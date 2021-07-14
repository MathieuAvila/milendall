#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "level.hxx"

#include "glmock.hpp"

#include "common.hxx"

using ::testing::_;
using ::testing::InSequence;

TEST(Level, LoadLevelOneRoom) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    Level* level = new Level(fl.getRoot().getSubPath("/level_single_room/level.json"));
    EXPECT_TRUE( level != nullptr );
}

TEST(Level, LoadLevel2Rooms1Gate) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    Level* level = new Level(fl.getRoot().getSubPath("/2_rooms_1_gate/level.json"));
    EXPECT_TRUE( level != nullptr );
}

TEST(Level, CheckRoomList) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    std::unique_ptr<Level> level = make_unique<Level>(fl.getRoot().getSubPath("/2_rooms_1_gate/level.json"));
    EXPECT_TRUE( level.get() != nullptr );

    auto room_list = level.get()->getRoomNames();
    EXPECT_TRUE( room_list.size() == 2);
    EXPECT_NE( std::find(room_list.begin(), room_list.end(), "room1"), room_list.end());
    EXPECT_NE( std::find(room_list.begin(), room_list.end(), "room2"), room_list.end());
}


