#include <gtest/gtest.h>

#include "file_library.hxx"

TEST(FILE_LIBRARY, GET_ROOT_INSTANCE){
    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto root = fl.getRoot();
    EXPECT_EQ( root.getPath(), "/" );
}

TEST(FILE_LIBRARY, Check_path_completion) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata");
    EXPECT_TRUE( toto_tata.getPath() == "/toto/tata" );
}

TEST(FILE_LIBRARY, Get_dir_path ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata").getSubPath("trou");
    EXPECT_TRUE( toto_tata.getDirPath().getPath() == "/toto/tata" );
}

TEST(FILE_LIBRARY, Check_path_completion_with_root) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("/tata/");
    EXPECT_TRUE( toto_tata.getPath() == "/tata" );
}

TEST(FILE_LIBRARY, Check_path_simplification) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata").getSubPath("..");
    EXPECT_TRUE( toto_tata.getPath() == "/toto" );
}

TEST(FILE_LIBRARY, List_directory) {
    // TODO
}

TEST(FILE_LIBRARY, Get_file_content_does_not_exist) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto no_file = fl.getRoot().getSubPath("no_entry");
    EXPECT_THROW(no_file.readContent(), LibraryException);
}

TEST(FILE_LIBRARY, Get_file_content_ok_simple_file) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto no_file = fl.getRoot().getSubPath("/toto/file.txt");
    auto content = no_file.readContent();

    EXPECT_TRUE( content.get() != nullptr );
    EXPECT_TRUE( content.get()->data() != nullptr );
    EXPECT_TRUE( content.get()->size()== 5 );
    EXPECT_TRUE( ((char*)(content.get()->data()))[0] == 'a' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[1] == 'b' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[2] == 'c' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[3] == 'd' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[4] == '\n' );
}

TEST(FILE_LIBRARY, File_content_in_2_dirs) {

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

    EXPECT_TRUE( content.get() != nullptr );
    EXPECT_TRUE( content.get()->data() != nullptr );
    EXPECT_TRUE( content.get()->size()== 5 );
    EXPECT_TRUE( ((char*)(content.get()->data()))[0] == 'a' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[1] == 'b' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[2] == 'c' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[3] == 'd' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[4] == '\n' );

    auto file2 = fl.getRoot().getSubPath("/toto/file2.txt");
    content = file2.readContent();

    EXPECT_TRUE( content.get() != nullptr );
    EXPECT_TRUE( content.get()->data() != nullptr );
    EXPECT_TRUE( content.get()->size()== 5 );
    EXPECT_TRUE( ((char*)(content.get()->data()))[0] == '0' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[1] == '1' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[2] == '2' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[3] == '3' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[4] == '\n' );


}

TEST(FILE_LIBRARY, File_content_conflict_in_2_dirs) {

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

    EXPECT_TRUE( content.get() != nullptr );
    EXPECT_TRUE( content.get()->data() != nullptr );
    EXPECT_TRUE( content.get()->size()== 5 );
    EXPECT_TRUE( ((char*)(content.get()->data()))[0] == 'a' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[1] == 'b' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[2] == 'c' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[3] == 'd' );
    EXPECT_TRUE( ((char*)(content.get()->data()))[4] == '\n' );
}

TEST(FILE_LIBRARY, List_directory_1_dir) {

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
    EXPECT_TRUE( content.size() == 2 );
    EXPECT_TRUE( content[0].getPath() == "/d1/toto/file1.txt" );
    EXPECT_TRUE( content[1].getPath() == "/d1/toto/file2.txt" );
}

TEST(FILE_LIBRARY, List_directory_2_dirs) {

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
    EXPECT_TRUE( content.size() == 4 );
    EXPECT_TRUE( content[0].getPath() == "/toto/file1.txt" );
    EXPECT_TRUE( content[1].getPath() == "/toto/file2.txt" );
    EXPECT_TRUE( content[2].getPath() == "/toto/file3.txt" );
    EXPECT_TRUE( content[3].getPath() == "/toto/file4.txt" );
}

TEST(FILE_LIBRARY, Get_string_file_content_does_not_exist) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto no_file = fl.getRoot().getSubPath("no_entry");
    auto content = no_file.readStringContent();

    EXPECT_TRUE( content == "" );
}

TEST(FILE_LIBRARY, Get_string_file_content_ok_simple_content) {

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto no_file = fl.getRoot().getSubPath("/toto/file.txt");
    auto content = no_file.readStringContent();

    EXPECT_TRUE( content == "abcd\n" );
}
