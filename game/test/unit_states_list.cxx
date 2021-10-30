#include <gtest/gtest.h>

#include "states_list.hxx"
#include "common.hxx"

static auto console = getConsole("unit_states_list");

class StatesListTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_states_list")->set_level(spdlog::level::debug);
     spdlog::get("states_list")->set_level(spdlog::level::debug);
  }
};

TEST_F(StatesListTest, run) {

    auto sl = StatesList{};

    ASSERT_EQ(sl.getState("void"), false);
    ASSERT_EQ(sl.getState("s1"), false);
    ASSERT_EQ(sl.getState("s2"), false);
    sl.setState("s1", false);
    sl.setState("s2", true);
    ASSERT_EQ(sl.getState("void"), false);
    ASSERT_EQ(sl.getState("s1"), false);
    ASSERT_EQ(sl.getState("s2"), true);
}
