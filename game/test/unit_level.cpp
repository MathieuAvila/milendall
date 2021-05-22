#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "level.hxx"

TEST(Level, LoadLevel) {

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/"));
    Level* level = new Level(fl.getRoot().getSubPath("/level_single_room/level.json"));
    EXPECT_TRUE( level != nullptr );
}
