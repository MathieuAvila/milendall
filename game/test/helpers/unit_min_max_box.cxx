#include <gtest/gtest.h>

#include "common.hxx"
#include "min_max_box.hxx"

#include <filesystem>

static auto console = getConsole("unit_min_max_box");

class MinMaxBoxTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_min_max_box")->set_level(spdlog::level::debug);
  }
};

TEST_F(MinMaxBoxTest, isIn) {
    MinMaxBox mmb(glm::vec3(0.0,  10.0, 100.0 ), glm::vec3(1.0,  11.0, 101.0 ));
    ASSERT_TRUE(mmb.isIn(glm::vec3(0.0,  10.0, 100.0))); // min
    ASSERT_TRUE(mmb.isIn(glm::vec3(1.0,  11.0, 101.0))); // max

    ASSERT_FALSE(mmb.isIn(glm::vec3(1.0 + 1.0,  11.0,       101.0       ))); // max + 1x
    ASSERT_FALSE(mmb.isIn(glm::vec3(1.0,        11.0 + 1.0, 101.0       ))); // max + 1y
    ASSERT_FALSE(mmb.isIn(glm::vec3(1.0,        11.0      , 101.0 + 1.0 ))); // max + 1z

    ASSERT_FALSE(mmb.isIn(glm::vec3(0.0 - 1.0,  10.0,       100.0       ))); // min - 1x
    ASSERT_FALSE(mmb.isIn(glm::vec3(0.0,        10.0 - 1.0, 100.0       ))); // min - 1y
    ASSERT_FALSE(mmb.isIn(glm::vec3(0.0,        10.0      , 100.0 - 1.0 ))); // min - 1z
}

TEST_F(MinMaxBoxTest, defined_isIn) {
    MinMaxBox mmb_false(false);
    ASSERT_FALSE(mmb_false.isIn(glm::vec3()));

    MinMaxBox mmb_true(true);
    ASSERT_TRUE(mmb_true.isIn(glm::vec3()));
}

TEST_F(MinMaxBoxTest, default_ctor) {
    MinMaxBox mmb_default;
    ASSERT_FALSE(mmb_default.isIn(glm::vec3()));
}
