#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "level.hxx"

#include "glmock.hpp"

#include "common.hxx"
#include <helper_math.hxx>

using ::testing::_;
using ::testing::InSequence;

static auto console = spdlog::stdout_color_mt("ut_level");

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

std::unique_ptr<Level> loadLevel(std::string level)
{
    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    auto l = make_unique<Level>(fl.getRoot().getSubPath("/" + level + "/level.json"));
    l->update(0.0f);
    return l;
}

TEST(Level, getDestinationPov_room1_r1r2_no_cross) {
    InSequence s; GLMock mock;

    // Check crossing of a IN portal
    auto level = loadLevel("2_rooms_1_gate");
    auto level_ptr = level.get();
    EXPECT_NE( level_ptr, nullptr );

    PointOfView origin{
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::mat4(),
        "room1"
    };
    PointOfView destination{
        glm::vec3(100.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::mat4(),
        "room1"
    };
    auto result = level_ptr->getDestinationPov(origin, destination.position);

    ASSERT_EQ(destination, result);
}

TEST(Level, getDestinationPov_room1_r1r2_cross) {
    InSequence s; GLMock mock;

    // Check crossing of a IN portal
    auto level = loadLevel("2_rooms_1_gate");
    auto level_ptr = level.get();
    EXPECT_NE( level_ptr, nullptr );

    PointOfView origin{
        glm::vec3(5.0, 2.0, 2.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::mat4(1.0f),
        "room1"
    };
    PointOfView destination{
        glm::vec3(5.0, 2.0, -2.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::mat4(1.0f),
        "room1"
    };

    auto result = level_ptr->getDestinationPov(origin, destination.position);

    console->info("{}", to_string(result));
    console->info("{}", mat4x4_to_string(result.local_reference));


    ASSERT_EQ(result, (PointOfView{
        glm::vec3(2.0, 2.0, 5.5),
        glm::vec3(0.0, 0.0, -1.0),
        glm::vec3(0.0, 0.0, -1.0),
        glm::mat4(  0.000000, 0.000000, -1.000000, 0.000000,
                    0.000000, 1.000000, 0.000000, 0.000000,
                    1.000000, 0.000000, 0.000000, 0.000000,
                    0.000000, 0.000000, 0.000000, 1.000000),
        "room2"}));
}

TEST(Level, getDestinationPov_room2_r1r2_cross___reversed) {
    InSequence s; GLMock mock;

    // Check crossing of a IN portal
    auto level = loadLevel("2_rooms_1_gate");
    auto level_ptr = level.get();
    EXPECT_NE( level_ptr, nullptr );

    PointOfView origin{
        glm::vec3(0.0, 2.0, 5.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::mat4(1.0f),
        "room2"
    };
    PointOfView destination{
        glm::vec3(5.0, 2.0, 5.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::mat4(1.0f),
        "room2"
    };

    auto result = level_ptr->getDestinationPov(origin, destination.position);

    console->info("{}", to_string(result));
    console->info("{}", mat4x4_to_string(result.local_reference));


    ASSERT_EQ(result, (PointOfView{
        glm::vec3(5.5, 2.0, 1.0),
        glm::vec3(0.0, 0.0, 1.0),
        glm::vec3(0.0, 0.0, 1.0),
        glm::mat4(  0.000000, 0.000000, 1.000000, 0.000000,
                    0.000000, 1.000000, 0.000000, 0.000000,
                    -1.000000, 0.000000, 0.000000, 0.000000,
                    0.000000, 0.000000, 0.000000, 1.000000),
        "room1"}));
}
