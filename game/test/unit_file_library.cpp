#include "catch_amalgamated.hpp"

#include "file_library.hxx"

TEST_CASE( "Get root instance", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto root = fl.getRoot();
    REQUIRE( root.getPath() == "/" );
}

TEST_CASE( "Check path completion", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata");
    REQUIRE( toto_tata.getPath() == "/toto/tata" );
}

TEST_CASE( "Check path completion with root", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("/tata/");
    REQUIRE( toto_tata.getPath() == "/tata" );
}

TEST_CASE( "Check path simplification", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata").getSubPath("..");
    REQUIRE( toto_tata.getPath() == "/toto" );
}

TEST_CASE("List directory", "[file_library]" ) {
    // TODO
}

TEST_CASE("Get file content does not exist", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto no_file = fl.getRoot().getSubPath("no_entry");
    auto content = no_file.readContent();

    REQUIRE( content.get() != nullptr );
    REQUIRE( content.get()->memory_block == nullptr );
}

TEST_CASE("Get file content ok, simple file", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto no_file = fl.getRoot().getSubPath("/toto/file.txt");
    auto content = no_file.readContent();

    REQUIRE( content.get() != nullptr );
    REQUIRE( content.get()->memory_block != nullptr );
    REQUIRE( content.get()->size == 5 );
    REQUIRE( ((char*)(content.get()->memory_block))[0] == 'a' );
    REQUIRE( ((char*)(content.get()->memory_block))[1] == 'b' );
    REQUIRE( ((char*)(content.get()->memory_block))[2] == 'c' );
    REQUIRE( ((char*)(content.get()->memory_block))[3] == 'd' );
    REQUIRE( ((char*)(content.get()->memory_block))[4] == '\n' );
}
