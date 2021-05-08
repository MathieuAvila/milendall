#include "catch_amalgamated.hpp"

#include <filesystem>
#include <iostream>

#include "level.hxx"

TEST_CASE( "Load level", "[level]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample/level_single_room"));
    Level* level = new Level(fl.getRoot().getSubPath("level.json"));
    REQUIRE( level != nullptr );
}
