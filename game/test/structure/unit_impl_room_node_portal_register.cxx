#include <gtest/gtest.h>

#include "impl_room_node_portal_register.hxx"
#include "common.hxx"
#include "level_exception.hxx"

using namespace std;

static auto console = getConsole("ut_impl_room_node_portal_register");

class ImplRoomNodePortalRegisterTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("ut_impl_room_node_portal_register")->set_level(spdlog::level::debug);
  }
};

TEST_F(ImplRoomNodePortalRegisterTest, init) {
    ImplRoomNodePortalRegister reg;
}

TEST_F(ImplRoomNodePortalRegisterTest, fill_and_find) {
    ImplRoomNodePortalRegister reg;
    reg.registerPortal(GateIdentifier("g1", "A"), (RoomNode*)0x1);
    reg.registerPortal(GateIdentifier("g1", "B"), (RoomNode*)0x2);

    reg.registerPortal(GateIdentifier("g2", "A"), (RoomNode*)0x3);
    reg.registerPortal(GateIdentifier("g2", "B"), (RoomNode*)0x4);

    ASSERT_EQ(reg.getPortal(GateIdentifier("g1", "A")), (RoomNode*)0x1);
    ASSERT_EQ(reg.getPortal(GateIdentifier("g1", "B")), (RoomNode*)0x2);
    ASSERT_EQ(reg.getPortal(GateIdentifier("g2", "A")), (RoomNode*)0x3);
    ASSERT_EQ(reg.getPortal(GateIdentifier("g2", "B")), (RoomNode*)0x4);
}

TEST_F(ImplRoomNodePortalRegisterTest, throw_error) {
    ImplRoomNodePortalRegister reg;
    reg.registerPortal(GateIdentifier("g1", "A"), (RoomNode*)0x1);
    reg.registerPortal(GateIdentifier("g1", "B"), (RoomNode*)0x2);
    reg.registerPortal(GateIdentifier("g2", "A"), (RoomNode*)0x3);
    reg.registerPortal(GateIdentifier("g2", "B"), (RoomNode*)0x4);

    ASSERT_THROW(reg.getPortal(GateIdentifier("gX", "A")), LevelException);
    ASSERT_THROW(reg.getPortal(GateIdentifier("g1", "X")), LevelException);

    ASSERT_THROW(reg.registerPortal(GateIdentifier("g1", "A"), (RoomNode*)0x4), LevelException);
}
