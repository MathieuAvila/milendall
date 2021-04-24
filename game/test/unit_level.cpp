#include "catch_amalgamated.hpp"

#include <filesystem>
#include <iostream>

#include "level.hxx"

TEST_CASE( "Load level", "[level]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_game_level");
    std::string pwd = std::filesystem::current_path();

    std::cout << pwd << std::endl;

    system(
        (std::string("rm -rf /tmp/test_game_level ; ")+
        "mkdir -p /tmp/test_game_level/ ;" +
        "cp " + pwd + "/../game/test/unit-level-single.json /tmp/test_game_level/level.json"
        ).c_str()
        );
    Level* level = new Level(fl.getRoot().getSubPath("level.json"));

    REQUIRE( level != nullptr );
}
