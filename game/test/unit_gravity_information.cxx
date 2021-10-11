#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "common.hxx"
#include "gravity_information.hxx"
#include <list>

using namespace testing;
using ::testing::_;

static auto console = getConsole("test_gravity_information");

class GravityInformationTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("test_gravity_information")->set_level(spdlog::level::debug);
  }
};

TEST_F(GravityInformationTest, default_init) {
    GravityInformation gravity;
    ASSERT_EQ(gravity.up, glm::vec3(0.0f, 1.0f, 0.0f));
    ASSERT_EQ(gravity.gravity, glm::vec3(0.0f, -1.0f, 0.0f));
    ASSERT_EQ(gravity.validity, 1000.0f);
    ASSERT_EQ(gravity.space_kind, GravityInformation::SpaceKind::GROUND);
}

TEST_F(GravityInformationTest, init_no_space_kind) {
    GravityInformation gravity(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 2000.0f);
    ASSERT_EQ(gravity.up, glm::vec3(1.0f, 0.0f, 0.0f));
    ASSERT_EQ(gravity.gravity, glm::vec3(0.0f, 10.0f, 0.0f));
    ASSERT_EQ(gravity.validity, 2000.0f);
    ASSERT_EQ(gravity.space_kind, GravityInformation::SpaceKind::GROUND);
}

TEST_F(GravityInformationTest, init_full) {
    GravityInformation gravity(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 2000.0f,
            GravityInformation::SpaceKind::FLY);
    ASSERT_EQ(gravity.up, glm::vec3(1.0f, 0.0f, 0.0f));
    ASSERT_EQ(gravity.gravity, glm::vec3(0.0f, 10.0f, 0.0f));
    ASSERT_EQ(gravity.validity, 2000.0f);
    ASSERT_EQ(gravity.space_kind, GravityInformation::SpaceKind::FLY);
}

TEST_F(GravityInformationTest, init_mixed_none) {

    std::list<GravityInformation> grav_list;

    GravityInformation gravity(grav_list);
    ASSERT_EQ(gravity.up, glm::vec3(0.0f, 1.0f, 0.0f));
    ASSERT_EQ(gravity.gravity, glm::vec3(0.0f, -1.0f, 0.0f));
    ASSERT_EQ(gravity.validity, 1000.0f);
    ASSERT_EQ(gravity.space_kind, GravityInformation::SpaceKind::GROUND);
}

TEST_F(GravityInformationTest, init_mixed_multiple) {

    std::list<GravityInformation> grav_list;
    grav_list.push_back(GravityInformation(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, 0.0f), 2.0f));
    grav_list.push_back(GravityInformation(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, 0.0f), 1.0f));
    grav_list.push_back(GravityInformation(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 3.0f));
    grav_list.push_back(GravityInformation(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 5.0f));

    GravityInformation gravity(grav_list);
    ASSERT_EQ(gravity.up, glm::vec3(-1.0f, 0.0f, 0.0f));
    ASSERT_EQ(gravity.gravity, glm::vec3(20.0f, 20.0f, 0.0f));
    ASSERT_EQ(gravity.validity, 1.0f);
    ASSERT_EQ(gravity.space_kind, GravityInformation::SpaceKind::GROUND);
}


TEST_F(GravityInformationTest, init_mixed_multiple_up_is_null) {

    std::list<GravityInformation> grav_list;
    grav_list.push_back(GravityInformation(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 2.0f));
    grav_list.push_back(GravityInformation(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f));

    GravityInformation gravity(grav_list);
    ASSERT_EQ(gravity.up, glm::vec3(0.0f, 1.0f, 0.0f));
    ASSERT_EQ(gravity.gravity, glm::vec3(0.0f, 30.0f, 0.0f));
    ASSERT_EQ(gravity.validity, 1.0f);
    ASSERT_EQ(gravity.space_kind, GravityInformation::SpaceKind::GROUND);
}



