#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "level_info.hxx"

#include "glmock.hpp"

#include "common.hxx"
#include <helper_math.hxx>

using ::testing::_;
using ::testing::InSequence;

static auto console = getConsole("ut_level_info");

class LevelInfoTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // spdlog::get("math")->set_level(spdlog::level::debug);
        // spdlog::get("face_list")->set_level(spdlog::level::debug);
        // spdlog::get("room_node")->set_level(spdlog::level::debug);
        spdlog::get("level_info")->set_level(spdlog::level::debug);
        spdlog::get("ut_level_info")->set_level(spdlog::level::debug);
    }
};

TEST_F(LevelInfoTest, LoadLevelOneRoom)
{
    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    auto level_info = make_unique<LevelInfo>(fl.getRoot().getSubPath("/level_info/level.json"));
    EXPECT_EQ(level_info->recommended_time, 10);
    EXPECT_EQ(level_info->name, "mon nom");
    EXPECT_EQ(level_info->section, "samples");
}
