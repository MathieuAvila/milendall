#include "catch_amalgamated.hpp"

#include "level.hxx"

TEST_CASE( "Load level", "[level]" ) {

    auto fl = FileLibrary();
    Level* level = new Level(fl.getRoot());

    REQUIRE( level != nullptr );
}
