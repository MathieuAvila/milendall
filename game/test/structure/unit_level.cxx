#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "level.hxx"

#include "glmock.hpp"

#include "common.hxx"
#include <helper_math.hxx>

using ::testing::_;
using ::testing::InSequence;

static auto console = getConsole("ut_level");

class LevelTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // spdlog::get("math")->set_level(spdlog::level::debug);
        // spdlog::get("face_list")->set_level(spdlog::level::debug);
        // spdlog::get("room_node")->set_level(spdlog::level::debug);
        spdlog::get("room")->set_level(spdlog::level::debug);
        spdlog::get("level")->set_level(spdlog::level::debug);
        spdlog::get("ut_level")->set_level(spdlog::level::debug);
    }
};

TEST_F(LevelTest, LoadLevelOneRoom)
{

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    Level *level = new Level(fl.getRoot().getSubPath("/level_single_room/level.json"));
    EXPECT_TRUE(level != nullptr);
}

TEST_F(LevelTest, GetDefinition)
{

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    Level *level = new Level(fl.getRoot().getSubPath("/2_rooms_1_gate/level.json"));
    EXPECT_TRUE(level != nullptr);

    auto def = level->getDefinition();
    EXPECT_EQ(def.start_room, "room1");
    EXPECT_EQ(def.end_room, "room2");
    EXPECT_EQ(def.recommended_time, 10);
    EXPECT_EQ(def.start_position, glm::vec3(1.0f, 2.0f, 3.0f));
}

TEST_F(LevelTest, LoadLevel2Rooms1Gate)
{

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    Level *level = new Level(fl.getRoot().getSubPath("/2_rooms_1_gate/level.json"));
    EXPECT_TRUE(level != nullptr);
}

TEST_F(LevelTest, CheckRoomList)
{

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../data/"));
    std::unique_ptr<Level> level = make_unique<Level>(fl.getRoot().getSubPath("/2_rooms_1_gate/level.json"));
    EXPECT_TRUE(level.get() != nullptr);

    auto room_list = level.get()->getRoomNames();
    EXPECT_TRUE(room_list.size() == 2);
    EXPECT_NE(std::find(room_list.begin(), room_list.end(), "room1"), room_list.end());
    EXPECT_NE(std::find(room_list.begin(), room_list.end(), "room2"), room_list.end());
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

TEST_F(LevelTest, getDestinationPov_room1_r1r2_no_cross)
{
    InSequence s;
    GLMock mock;

    // Check crossing of a IN portal
    auto level = loadLevel("2_rooms_1_gate");
    auto level_ptr = level.get();
    EXPECT_NE(level_ptr, nullptr);

    PointOfView origin{
        glm::vec3(0.0, 0.0, 0.0),
        glm::mat4(),
        "room1"};
    PointOfView destination{
        glm::vec3(100.0, 0.0, 0.0),
        glm::mat4(),
        "room1"};
    auto result = level_ptr->getDestinationPov(origin, destination.position);

    ASSERT_EQ(destination, result);
}

TEST_F(LevelTest, getDestinationPov_room1_r1r2_cross)
{
    InSequence s;
    GLMock mock;

    // Check crossing of a IN portal
    auto level = loadLevel("2_rooms_1_gate");
    auto level_ptr = level.get();
    EXPECT_NE(level_ptr, nullptr);

    PointOfView origin{
        glm::vec3(4.7, 1.0, 2.0),
        glm::mat4(1.0f),
        "room1"};
    PointOfView destination{
        glm::vec3(4.7, 1.0, -2.0),
        glm::mat4(1.0f),
        "room1"};

    auto result = level_ptr->getDestinationPov(origin, destination.position);

    console->info("{}", to_string(result));

    ASSERT_EQ(result, (PointOfView{
                          glm::vec3(2.0, 1.0, 2.2),
                          glm::mat4(0.000000, 0.000000, 1.000000, 0.000000,
                                    0.000000, 1.000000, 0.000000, 0.000000,
                                    -1.000000, 0.000000, 0.000000, 0.000000,
                                    0.000000, 0.000000, 0.000000, 0.000000),
                          "room2"}));
}

TEST_F(LevelTest, isWallReached_0_no_hit)
{
    InSequence s;
    GLMock mock;
    auto level = loadLevel("2_rooms_1_gate");

    PointOfView origin{
        glm::vec3(4.7, 1.0, 2.0),
        glm::mat4(1.0f),
        "room1"};
    glm::vec3 destination(4.7, 1.0, 1.9);
    float radius = 1.0f;
    PointOfView endPoint;
    glm::vec3 vectorEndPoint, destinationEndPoint;
    glm::vec3 normal;
    float distance;
    FaceHard *face;
    bool portalCrossed;

    bool hit = level->isWallReached(
        origin,
        destination,
        radius,
        endPoint, vectorEndPoint, destinationEndPoint,
        normal,
        distance,
        face,
        portalCrossed);
    console->debug("vectorEndPoint = {}", vec3_to_string(vectorEndPoint));
    ASSERT_FALSE(hit);
    ASSERT_FALSE(portalCrossed);
    ASSERT_EQ(endPoint, (PointOfView{
                            glm::vec3(4.7, 1.0, 1.9),
                            glm::mat4(1.0f),
                            "room1"}));
    ASSERT_FLOAT_EQ(distance, 0.1);
    ASSERT_EQ(vectorEndPoint, glm::vec3(0.0, 0.0, -1.0));
    ASSERT_EQ(destinationEndPoint, destination);
}

TEST_F(LevelTest, isWallReached_1_hit_ground_no_matrix_change)
{
    InSequence s;
    GLMock mock;
    auto level = loadLevel("2_rooms_1_gate");

    PointOfView origin{
        glm::vec3(5.0, 2.0, 3.0),
        glm::mat4(1.0f),
        "room1"};
    glm::vec3 destination(5.0, -1.0, 3.0);
    float radius = 1.0f;
    PointOfView endPoint;
    glm::vec3 vectorEndPoint, destinationEndPoint;
    glm::vec3 normal;
    float distance;
    FaceHard *face;
    bool portalCrossed;

    bool hit = level->isWallReached(
        origin,
        destination,
        radius,
        endPoint, vectorEndPoint, destinationEndPoint,
        normal,
        distance,
        face,
        portalCrossed);
    console->debug("endPoint = {}", to_string(endPoint));
    console->debug("vectorEndPoint = {}", vec3_to_string(vectorEndPoint));
    console->debug("distance = {}", distance);
    ASSERT_TRUE(hit);
    ASSERT_FALSE(portalCrossed);
    ASSERT_EQ(endPoint, (PointOfView{
                            glm::vec3(5.0, 1.0, 3.0),
                            glm::mat4(1.0f),
                            "room1"}));
    ASSERT_EQ(destinationEndPoint, destination);
    ASSERT_EQ(vectorEndPoint, glm::vec3(0.0, -1.0, 0.0));
    ASSERT_EQ(normal, glm::vec3(.0, 1.0, .0));
    ASSERT_FLOAT_EQ(distance, 1.0f);
    ASSERT_NE(face, nullptr);
}

TEST_F(LevelTest, isWallReached_2_hit_wall_matrix_change)
{

    // hit a wall on a subobject

    InSequence s;
    GLMock mock;
    auto level = loadLevel("2_rooms_1_gate");

    PointOfView origin{
        glm::vec3(5.5, 1.0, 1.9),
        glm::mat4(1.0f),
        "room1"};
    glm::vec3 destination(5.5, 1.0, -1.9);
    float radius = 1.0f;
    PointOfView endPoint;
    glm::vec3 vectorEndPoint, destinationEndPoint;
    glm::vec3 normal;
    float distance;
    FaceHard *face;
    bool portalCrossed;

    bool hit = level->isWallReached(
        origin,
        destination,
        radius,
        endPoint, vectorEndPoint, destinationEndPoint,
        normal,
        distance,
        face,
        portalCrossed);
    console->debug("endPoint = {}", to_string(endPoint));
    console->debug("vectorEndPoint = {}", vec3_to_string(vectorEndPoint));
    console->debug("distance = {}", distance);
    ASSERT_TRUE(hit);
    ASSERT_FALSE(portalCrossed);
    ASSERT_EQ(endPoint, (PointOfView{
                            glm::vec3(5.5f, 1.0f, 1.2f),
                            glm::mat4(1.0f),
                            "room1"}));

    console->debug("Check wall normal {}", vec3_to_string(normal));
    console->debug("endPoint = {}", to_string(endPoint));
    console->debug("vectorEndPoint = {}", vec3_to_string(vectorEndPoint));
    console->debug("destinationEndPoint = {}", vec3_to_string(destinationEndPoint));
    console->debug("distance = {}", distance);

    ASSERT_EQ(vectorEndPoint, glm::vec3(.0f, .0f, -1.0f));
    ASSERT_EQ(destinationEndPoint, destination);

    ASSERT_TRUE(glm::length(normal - glm::vec3(.0f, .0f, 1.0f)) < 0.01f);
    ASSERT_FLOAT_EQ(distance, 0.7f);
    ASSERT_NE(face, nullptr);
}

TEST_F(LevelTest, isWallReached_3_no_hit_cross_portal)
{
    InSequence s;
    GLMock mock;
    auto level = loadLevel("2_rooms_1_gate");

    PointOfView origin{
        glm::vec3(5.25, 1.0, 3.0),
        glm::mat4(1.0f),
        "room1"};
    glm::vec3 destination(5.25, 1.0, -0.5);
    float radius = 0.2f;
    PointOfView endPoint;
    glm::vec3 vectorEndPoint, destinationEndPoint;
    glm::vec3 normal(0.0f);
    float distance;
    FaceHard *face;
    bool portalCrossed;

    bool hit = level->isWallReached(
        origin,
        destination,
        radius,
        endPoint, vectorEndPoint, destinationEndPoint,
        normal,
        distance,
        face,
        portalCrossed);
    console->debug("endPoint = {}", vec3_to_string(endPoint.position));
    console->debug("vectorEndPoint = {}", vec3_to_string(vectorEndPoint));
    console->debug("destinationEndPoint = {}", vec3_to_string(destinationEndPoint));
    console->debug("normal = {}", vec3_to_string(normal));
    console->debug("distance = {}", distance);
    ASSERT_FALSE(hit);
    ASSERT_TRUE(portalCrossed);
    ASSERT_EQ(distance, 3.5f);
    ASSERT_EQ(endPoint.position, glm::vec3(0.5f, 1.0f, 2.75f));
    ASSERT_EQ(vectorEndPoint, glm::vec3(1.0f, 0.0f, 0.0f));
    ASSERT_EQ(destinationEndPoint, glm::vec3(0.5, 1.0, 2.75));
    ASSERT_EQ(normal, glm::vec3(0.0f, 0.0f, 0.0f));
    ASSERT_EQ(endPoint.room, "room2");
}

TEST_F(LevelTest, isWallReached_4_hit_cross_portal_wall_no_matrix_change)
{

    // will hit the wall on the other side of portal
    InSequence s;
    GLMock mock;
    auto level = loadLevel("2_rooms_1_gate");

    PointOfView origin{
        glm::vec3(4.7f, 1.0f, 2.0f),
        glm::mat4(1.0f),
        "room1"};
    glm::vec3 destination(4.7f, 1.0f, -15.0f); // hit the wall
    float radius = 0.2f;
    PointOfView endPoint;
    glm::vec3 vectorEndPoint, destinationEndPoint;
    glm::vec3 normal;
    float distance;
    FaceHard *face;
    bool portalCrossed;

    bool hit = level->isWallReached(
        origin,
        destination,
        radius,
        endPoint, vectorEndPoint, destinationEndPoint,
        normal,
        distance,
        face,
        portalCrossed);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("endPoint = {}", to_string(endPoint));
    console->info("destinationEndPoint = {}", vec3_to_string(destinationEndPoint));
    console->info("distance = {}", to_string(distance));
    console->info("normal = {}", vec3_to_string(normal));
    ASSERT_TRUE(hit);
    ASSERT_TRUE(portalCrossed);
    ASSERT_TRUE(glm::length(endPoint.position - glm::vec3(9.8f, 1.0f, 2.2f)) < 0.01f);
    ASSERT_FLOAT_EQ(distance, 11.8f);
    ASSERT_EQ(endPoint.room, "room2");
    ASSERT_TRUE(glm::length(normal - glm::vec3(-1.0f, .0, .0)) < 0.01f);
}

TEST_F(LevelTest, isWallReached_5_hit_cross_portal_wall_matrix_change)
{
    InSequence s;
    GLMock mock;
    auto level = loadLevel("2_rooms_1_gate");

    // pwa, grosse flemme.
}

TEST_F(LevelTest, isWallReached_6_border_hit)
{

    // special case where it's a hit on the border.

    InSequence s;
    GLMock mock;
    auto level = loadLevel("2_rooms_1_gate");

    PointOfView origin(
        glm::vec3(4.7f, 1.0f, 2.0f),
        glm::mat4(1.0f),
        "room2");
    glm::vec3 destination(4.7f, 2.0f, -2.0f); // hit the wall of the gate, on the border
    float radius = 0.7f;
    PointOfView endPoint;
    glm::vec3 vectorEndPoint, destinationEndPoint;
    glm::vec3 normal;
    float distance;
    FaceHard *face;
    bool portalCrossed;

    bool hit = level->isWallReached(
        origin,
        destination,
        radius,
        endPoint, vectorEndPoint, destinationEndPoint,
        normal,
        distance,
        face,
        portalCrossed);
    console->info("Check wall normal {}", vec3_to_string(normal));
    console->info("endPoint = {}", to_string(endPoint));
    console->info("distance = {}", distance);
    ASSERT_TRUE(hit);
    ASSERT_FALSE(portalCrossed);
    ASSERT_TRUE(glm::length(endPoint.position - glm::vec3(4.7 , 1.325 , 0.7)) < 0.01f);
    ASSERT_TRUE(abs(distance - 1.34f) < 0.01f);
    ASSERT_EQ(endPoint.room, "room2");
    ASSERT_TRUE(glm::length(normal - glm::vec3(0.0f, 0.0f, 1.0f)) < 0.01f);
}


TEST_F(LevelTest, getRoomMeshMatrix)
{
    InSequence s;
    GLMock mock;
    auto level = loadLevel("2_rooms_1_gate");

    glm::mat4 mat = level->getRoomMeshMatrix("room1", "b0_root_door0");
    console->debug("mat = {}", mat4x4_to_string(mat));
    ASSERT_EQ(mat,
        glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            4.0f, 0.0f, 0.0f, 1.0f));
}

