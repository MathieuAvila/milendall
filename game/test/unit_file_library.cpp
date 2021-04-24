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

TEST_CASE( "Get dir path", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata").getSubPath("trou");
    REQUIRE( toto_tata.getDirPath().getPath() == "/toto/tata" );
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
    REQUIRE( content.get()->size == 0 );
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

TEST_CASE("File content in 2 dirs", "[file_library]") {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library/d1");
    fl.addRootFilesystem("/tmp/test_file_library/d2");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/d1/toto ;"
        "mkdir -p /tmp/test_file_library/d2/toto ;"
        "echo abcd > /tmp/test_file_library/d1/toto/file1.txt ; "
        "echo 0123 > /tmp/test_file_library/d2/toto/file2.txt"
        );
    auto file1 = fl.getRoot().getSubPath("/toto/file1.txt");
    auto content = file1.readContent();

    REQUIRE( content.get() != nullptr );
    REQUIRE( content.get()->memory_block != nullptr );
    REQUIRE( content.get()->size == 5 );
    REQUIRE( ((char*)(content.get()->memory_block))[0] == 'a' );
    REQUIRE( ((char*)(content.get()->memory_block))[1] == 'b' );
    REQUIRE( ((char*)(content.get()->memory_block))[2] == 'c' );
    REQUIRE( ((char*)(content.get()->memory_block))[3] == 'd' );
    REQUIRE( ((char*)(content.get()->memory_block))[4] == '\n' );

    auto file2 = fl.getRoot().getSubPath("/toto/file2.txt");
    content = file2.readContent();

    REQUIRE( content.get() != nullptr );
    REQUIRE( content.get()->memory_block != nullptr );
    REQUIRE( content.get()->size == 5 );
    REQUIRE( ((char*)(content.get()->memory_block))[0] == '0' );
    REQUIRE( ((char*)(content.get()->memory_block))[1] == '1' );
    REQUIRE( ((char*)(content.get()->memory_block))[2] == '2' );
    REQUIRE( ((char*)(content.get()->memory_block))[3] == '3' );
    REQUIRE( ((char*)(content.get()->memory_block))[4] == '\n' );


}

TEST_CASE("File content conflict in 2 dirs", "[file_library]") {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library/d1");
    fl.addRootFilesystem("/tmp/test_file_library/d2");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/d1/toto ;"
        "mkdir -p /tmp/test_file_library/d2/toto ;"
        "echo abcd > /tmp/test_file_library/d1/toto/file.txt ; "
        "echo 0123 > /tmp/test_file_library/d2/toto/file.txt"
        );
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

TEST_CASE("List directory 1 dir", "[file_library]") {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/d1/toto ;"
        "echo abcd > /tmp/test_file_library/d1/toto/file1.txt ; "
        "echo 0123 > /tmp/test_file_library/d1/toto/file2.txt"
        );
    auto dir1 = fl.getRoot().getSubPath("/d1/toto");
    auto content = dir1.listDirectory();
    REQUIRE( content.size() == 2 );
    REQUIRE( content[0].getPath() == "/d1/toto/file1.txt" );
    REQUIRE( content[1].getPath() == "/d1/toto/file2.txt" );
}

TEST_CASE("List directory 2 dirs", "[file_library]") {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library/d1");
    fl.addRootFilesystem("/tmp/test_file_library/d2");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/d1/toto ;"
        "mkdir -p /tmp/test_file_library/d2/toto ;"
        "echo abcd > /tmp/test_file_library/d1/toto/file1.txt ; "
        "echo 0123 > /tmp/test_file_library/d1/toto/file2.txt ; "
        "echo abcd > /tmp/test_file_library/d2/toto/file3.txt ; "
        "echo 0123 > /tmp/test_file_library/d2/toto/file4.txt ; "
        );
    auto dir1 = fl.getRoot().getSubPath("/toto");
    auto content = dir1.listDirectory();
    REQUIRE( content.size() == 4 );
    REQUIRE( content[0].getPath() == "/toto/file1.txt" );
    REQUIRE( content[1].getPath() == "/toto/file2.txt" );
    REQUIRE( content[2].getPath() == "/toto/file3.txt" );
    REQUIRE( content[3].getPath() == "/toto/file4.txt" );
}

TEST_CASE("Get string file content does not exist", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto no_file = fl.getRoot().getSubPath("no_entry");
    auto content = no_file.readStringContent();

    REQUIRE( content == "" );
}

TEST_CASE("Get string file content ok, simple content", "[file_library]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto no_file = fl.getRoot().getSubPath("/toto/file.txt");
    auto content = no_file.readStringContent();

    REQUIRE( content == "abcd\n" );
}