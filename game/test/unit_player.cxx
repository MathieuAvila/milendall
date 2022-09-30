#include <gtest/gtest.h>

#include <iostream>
#include <memory>

#include "common.hxx"
#include "player.hxx"



using namespace testing;

static auto console = getConsole("test_player");

class PlayerTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("player")->set_level(spdlog::level::debug);
     spdlog::get("test_player")->set_level(spdlog::level::debug);
  }
};

TEST_F(PlayerTest, set_actions) {

    std::unique_ptr<Player> player = std::make_unique<Player>();
    Player::ActionSet actions;

    ASSERT_FLOAT_EQ(player->currentActions.verticalAngle, 0.0f);
    ASSERT_FLOAT_EQ(player->previous_vertical_angle, 0.0f);

    actions.verticalAngle = 1.0;
    player->setActionSet(actions);
    ASSERT_FLOAT_EQ(player->currentActions.verticalAngle, 1.0f);

    actions.verticalAngle = -1.0;
    player->setActionSet(actions);
    ASSERT_FLOAT_EQ(player->currentActions.verticalAngle, -1.0f);

    actions.verticalAngle = -10.0;
    player->setActionSet(actions);
    ASSERT_FLOAT_EQ(player->currentActions.verticalAngle, -3.14f * 3.5f / 10.0f); // check cap

    actions.verticalAngle = 10.0;
    player->setActionSet(actions);
    ASSERT_FLOAT_EQ(player->currentActions.verticalAngle, 3.14f * 3.5f / 10.0f); // check cap

}
