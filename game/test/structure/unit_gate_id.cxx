#include <gtest/gtest.h>

#include "gate_id.hxx"
#include "common.hxx"

using namespace std;

static auto console = getConsole("ut_gate_id");

class GateIdTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("ut_gate_id")->set_level(spdlog::level::debug);
  }
};

TEST_F(GateIdTest, init) {
    GateIdentifier g;
}

TEST_F(GateIdTest, init_values) {
    GateIdentifier g("g", "c");
}

TEST_F(GateIdTest, equality) {
    EXPECT_EQ(GateIdentifier("g", "c"), GateIdentifier("g", "c"));
    EXPECT_NE(GateIdentifier("g", "c"), GateIdentifier("g", "c2"));
    EXPECT_NE(GateIdentifier("g", "c"), GateIdentifier("g2", "c"));
}

TEST_F(GateIdTest, print) {
    EXPECT_EQ(to_string(GateIdentifier("g", "c")), "g:c");
}
