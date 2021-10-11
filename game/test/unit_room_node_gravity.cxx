#include <gtest/gtest.h>

#include "common.hxx"
#include "room_node_gravity.hxx"
#include "script.hxx"
#include <filesystem>
#include "helper_math.hxx"

static auto console = getConsole("unit_room_node_gravity");

using namespace std;

class RoomNodeGravityTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_room_node_gravity")->set_level(spdlog::level::debug);
     spdlog::get("room_node_gravity")->set_level(spdlog::level::debug);
     spdlog::get("script")->set_level(spdlog::level::debug);
  }
};

TEST_F(RoomNodeGravityTest, get_none) {

    RoomNodeGravity gravity("my_name", nullptr);
    GravityInformation result;
    ASSERT_FALSE(gravity.getGravityInformation(glm::vec3(), glm::vec3(), 0.0, 0.0, 0.0, result));
}

TEST_F(RoomNodeGravityTest, get_default_unboxed) {

    RoomNodeGravity gravity("my_name", nullptr);
    GravityInformation result;
    auto my_json = json::parse("{ \"default\" : { \"gravity\": [ 1.0, 2.0, 3.0] ,  \"up\": [ 4.0, 5.0, 6.0]  } }");

    gravity.readParameters(my_json);
    ASSERT_TRUE(gravity.getGravityInformation(glm::vec3(), glm::vec3(), 0.0, 0.0, 0.0, result));
    ASSERT_EQ(result.gravity, glm::vec3(1.0, 2.0, 3.0));
    ASSERT_EQ(result.up, glm::vec3(4.0, 5.0, 6.0));
}

TEST_F(RoomNodeGravityTest, get_default_boxed) {

    RoomNodeGravity gravity("my_name", nullptr);
    GravityInformation result;
    auto my_json = json::parse(
        "{"
            "\"default\" : { \"gravity\": [ 1.0, 2.0, 3.0] ,  \"up\": [ 4.0, 5.0, 6.0]  } , "
            "\"box\": { \"min\" : [ 10.0, 20.0, 30.0] ,  \"max\" : [ 40.0, 50.0, 60.0]  }"
        "}");

    gravity.readParameters(my_json);

    ASSERT_TRUE(gravity.getGravityInformation(glm::vec3(11.0, 21.0, 31.0), glm::vec3(), 0.0, 0.0, 0.0, result));
    ASSERT_EQ(result.gravity, glm::vec3(1.0, 2.0, 3.0));
    ASSERT_EQ(result.up, glm::vec3(4.0, 5.0, 6.0));

    ASSERT_FALSE(gravity.getGravityInformation(glm::vec3(0.0, 0.0, 0.0), glm::vec3(), 0.0, 0.0, 0.0, result));
}

TEST_F(RoomNodeGravityTest, get_script_unboxed) {

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/gravity"));
    Script testScript(fl.getRoot().getSubPath("/simple.lua"));
    RoomNodeGravity gravity("my_name", &testScript);
    GravityInformation result;
    auto my_json = json::parse("{ }");

    gravity.readParameters(my_json);
    ASSERT_TRUE(gravity.getGravityInformation(glm::vec3(10.0, 100.0, 1000.0), glm::vec3(20.0, 200.0, 2000.0), 30.0, 300.0, 3000.0, result));
    console->debug("{}", vec3_to_string(result.gravity));
    console->debug("{}", vec3_to_string(result.up));
    console->debug("{}", result.validity);
    ASSERT_EQ(result.gravity, glm::vec3(11.0, 101.0, 1001.0));
    ASSERT_EQ(result.up, glm::vec3(21.0, 201.0, 2001.0));
    ASSERT_EQ(result.validity, 3330.0);
}