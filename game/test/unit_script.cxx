#include <gtest/gtest.h>

#include "common.hxx"
#include "script.hxx"

#include <filesystem>

static auto console = getConsole("unit_script");

class ScriptTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_script")->set_level(spdlog::level::debug);
     spdlog::get("script")->set_level(spdlog::level::debug);
  }
};

TEST_F(ScriptTest, run) {

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/script"));

    Script testScript(fl.getRoot().getSubPath("/simple.lua"));
    Script::ValueTable values;
    values.insert(std::pair<std::string, float>("key1", 1.0f));
    values.insert(std::pair<std::string, float>("key2", 0.1f));
    auto result = testScript.run("testrun", values);

    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.count("key1"), 1);
    ASSERT_EQ(result.count("key2"), 1);

    ASSERT_EQ(result["key1"], 2.0f);
}


TEST_F(ScriptTest, two_runs) {

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/script"));

    {
    Script testScriptGhost(fl.getRoot().getSubPath("/simple.lua"));
    Script::ValueTable values;
    auto result = testScriptGhost.run("testrun", values);
    }

    Script testScript(fl.getRoot().getSubPath("/simple.lua"));
    Script::ValueTable values;
    values.insert(std::pair<std::string, float>("key1", 1.0f));
    values.insert(std::pair<std::string, float>("key2", 0.1f));
    auto result = testScript.run("testrun", values);

    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.count("key1"), 1);
    ASSERT_EQ(result.count("key2"), 1);

    ASSERT_EQ(result["key1"], 2.0f);
}
