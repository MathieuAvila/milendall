#include <gtest/gtest.h>

#include "file_library.hxx"
#include <functional>
#include <algorithm>

TEST(FILE_LIBRARY, GET_ROOT_INSTANCE)
{
    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto root = fl.getRoot();
    EXPECT_EQ(root.getPath(), "/");
}

TEST(FILE_LIBRARY, Check_path_completion)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata");
    EXPECT_TRUE(toto_tata.getPath() == "/toto/tata");
}

TEST(FILE_LIBRARY, Get_dir_path)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata").getSubPath("trou");
    EXPECT_TRUE(toto_tata.getDirPath().getPath() == "/toto/tata");
}

TEST(FILE_LIBRARY, Check_path_completion_with_root)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("/tata/");
    EXPECT_TRUE(toto_tata.getPath() == "/tata");
}

TEST(FILE_LIBRARY, Check_path_simplification)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto toto_tata = fl.getRoot().getSubPath("toto").getSubPath("tata").getSubPath("..");
    EXPECT_TRUE(toto_tata.getPath() == "/toto");
}

TEST(FILE_LIBRARY, isFile)
{
    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "mkdir /tmp/test_file_library/toto/sbdir; "
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto real_file = fl.getRoot().getSubPath("/toto/file.txt");
    EXPECT_TRUE(real_file.isFile());
    auto no_real_file = fl.getRoot().getSubPath("/toto/no_file.txt");
    EXPECT_FALSE(no_real_file.isFile());
    auto is_dir = fl.getRoot().getSubPath("/toto/sbdir");
    EXPECT_FALSE(is_dir.isFile());
}

TEST(FILE_LIBRARY, isDirectory)
{
    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "mkdir /tmp/test_file_library/toto/sbdir; "
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto real_file = fl.getRoot().getSubPath("/toto/file.txt");
    EXPECT_FALSE(real_file.isDirectory());
    auto no_real_file = fl.getRoot().getSubPath("/toto/no_file.txt");
    EXPECT_FALSE(no_real_file.isDirectory());
    auto is_dir = fl.getRoot().getSubPath("/toto/sbdir");
    EXPECT_TRUE(is_dir.isDirectory());
}

TEST(FILE_LIBRARY, List_directory)
{
    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "mkdir -p /tmp/test_file_library/toto/sbdir; "
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto file_list = fl.getRoot().getSubPath("/toto").listDirectory();
    EXPECT_EQ(file_list.size(), 2);
    std::sort(file_list.begin(), file_list.end());
    EXPECT_EQ(file_list[0].getFileName(), "file.txt");
    EXPECT_EQ(file_list[1].getFileName(), "sbdir");
}

TEST(FILE_LIBRARY, Get_file_content_does_not_exist)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto no_file = fl.getRoot().getSubPath("no_entry");
    EXPECT_THROW(no_file.readContent(), LibraryException);
}

TEST(FILE_LIBRARY, Get_file_content_ok_simple_file)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto no_file = fl.getRoot().getSubPath("/toto/file.txt");
    auto content = no_file.readContent();

    EXPECT_NE(content.get(), nullptr);
    EXPECT_NE(content.get()->data(), nullptr);
    EXPECT_EQ(content.get()->size(), 5);
    EXPECT_EQ(((char *)(content.get()->data()))[0], 'a');
    EXPECT_EQ(((char *)(content.get()->data()))[1], 'b');
    EXPECT_EQ(((char *)(content.get()->data()))[2], 'c');
    EXPECT_EQ(((char *)(content.get()->data()))[3], 'd');
    EXPECT_EQ(((char *)(content.get()->data()))[4], '\n');
}

TEST(FILE_LIBRARY, File_content_in_2_dirs)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library/d1");
    fl.addRootFilesystem("/tmp/test_file_library/d2");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/d1/toto ;"
        "mkdir -p /tmp/test_file_library/d2/toto ;"
        "echo abcd > /tmp/test_file_library/d1/toto/file1.txt ; "
        "echo 0123 > /tmp/test_file_library/d2/toto/file2.txt");
    auto file1 = fl.getRoot().getSubPath("/toto/file1.txt");
    auto content = file1.readContent();

    EXPECT_NE(content.get(), nullptr);
    EXPECT_NE(content.get()->data(), nullptr);
    EXPECT_EQ(content.get()->size(), 5);
    EXPECT_EQ(((char *)(content.get()->data()))[0], 'a');
    EXPECT_EQ(((char *)(content.get()->data()))[1], 'b');
    EXPECT_EQ(((char *)(content.get()->data()))[2], 'c');
    EXPECT_EQ(((char *)(content.get()->data()))[3], 'd');
    EXPECT_EQ(((char *)(content.get()->data()))[4], '\n');

    auto file2 = fl.getRoot().getSubPath("/toto/file2.txt");
    content = file2.readContent();

    EXPECT_NE(content.get(), nullptr);
    EXPECT_NE(content.get()->data(), nullptr);
    EXPECT_EQ(content.get()->size(), 5);
    EXPECT_EQ(((char *)(content.get()->data()))[0], '0');
    EXPECT_EQ(((char *)(content.get()->data()))[1], '1');
    EXPECT_EQ(((char *)(content.get()->data()))[2], '2');
    EXPECT_EQ(((char *)(content.get()->data()))[3], '3');
    EXPECT_EQ(((char *)(content.get()->data()))[4], '\n');
}

TEST(FILE_LIBRARY, File_content_conflict_in_2_dirs)
{
    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library/d1");
    fl.addRootFilesystem("/tmp/test_file_library/d2");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/d1/toto ;"
        "mkdir -p /tmp/test_file_library/d2/toto ;"
        "echo abcd > /tmp/test_file_library/d1/toto/file.txt ; "
        "echo 0123 > /tmp/test_file_library/d2/toto/file.txt");
    auto no_file = fl.getRoot().getSubPath("/toto/file.txt");
    auto content = no_file.readContent();

    EXPECT_NE(content.get(), nullptr);
    EXPECT_NE(content.get()->data(), nullptr);
    EXPECT_EQ(content.get()->size(), 5);
    EXPECT_EQ(((char *)(content.get()->data()))[0], 'a');
    EXPECT_EQ(((char *)(content.get()->data()))[1], 'b');
    EXPECT_EQ(((char *)(content.get()->data()))[2], 'c');
    EXPECT_EQ(((char *)(content.get()->data()))[3], 'd');
    EXPECT_EQ(((char *)(content.get()->data()))[4], '\n');
}

TEST(FILE_LIBRARY, List_directory_1_dir)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/d1/toto ;"
        "echo abcd > /tmp/test_file_library/d1/toto/file1.txt ; "
        "echo 0123 > /tmp/test_file_library/d1/toto/file2.txt");
    system(
        "find /tmp/test_file_library"); // for debug purposes
    auto dir1 = fl.getRoot().getSubPath("/d1/toto");
    auto content = dir1.listDirectory();
    EXPECT_EQ(content.size(), 2);
    std::sort(content.begin(), content.end());
    EXPECT_EQ(content[0].getPath(), "/d1/toto/file1.txt");
    EXPECT_EQ(content[1].getPath(), "/d1/toto/file2.txt");
}

TEST(FILE_LIBRARY, List_directory_2_dirs)
{

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
        "echo 0123 > /tmp/test_file_library/d2/toto/file4.txt ; ");
    auto dir1 = fl.getRoot().getSubPath("/toto");
    auto content = dir1.listDirectory();
    EXPECT_TRUE(content.size() == 4);
    std::sort(content.begin(), content.end());
    EXPECT_TRUE(content[0].getPath() == "/toto/file1.txt");
    EXPECT_TRUE(content[1].getPath() == "/toto/file2.txt");
    EXPECT_TRUE(content[2].getPath() == "/toto/file3.txt");
    EXPECT_TRUE(content[3].getPath() == "/toto/file4.txt");
}

TEST(FILE_LIBRARY, Get_string_file_content_does_not_exist)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    auto no_file = fl.getRoot().getSubPath("no_entry");
    EXPECT_THROW(no_file.readStringContent(), LibraryException);
}

TEST(FILE_LIBRARY, Get_string_file_content_ok_simple_content)
{

    auto fl = FileLibrary();
    fl.addRootFilesystem("/tmp/test_file_library");
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/toto ;"
        "echo abcd > /tmp/test_file_library/toto/file.txt");
    auto no_file = fl.getRoot().getSubPath("/toto/file.txt");
    auto content = no_file.readStringContent();

    EXPECT_EQ(content, "abcd\n");
}

TEST(FILE_LIBRARY, search_file_1_in_root)
{

    auto fl = FileLibrary();
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library ; "
        "echo abcd > /tmp/test_file_library/file1.txt ; "
        "echo 0123 > /tmp/test_file_library/file2.txt ; "
        "echo abcd > /tmp/test_file_library/file3.txt ; "
        "echo 0123 > /tmp/test_file_library/file4.txt ; ");
    fl.addRootFilesystem("/tmp/test_file_library");
    auto results = fl.searchFile("file2.txt");
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results.begin()->getPath(), "/file2.txt");
}

TEST(FILE_LIBRARY, search_file_2_in_2_dirs)
{

    auto fl = FileLibrary();
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/dir1 ; "
        "mkdir -p /tmp/test_file_library/dir2 ; "
        "touch /tmp/test_file_library/file1.txt ; "
        "touch /tmp/test_file_library/dir1/match.txt ; "
        "touch /tmp/test_file_library/dir2/match.txt ; "
        "touch /tmp/test_file_library/file3.txt ; "
        "touch /tmp/test_file_library/file4.txt ; ");
    fl.addRootFilesystem("/tmp/test_file_library");
    auto results = fl.searchFile("match.txt");
    EXPECT_EQ(results.size(), 2);
    auto first = results.begin();
    auto second = first;
    second++;
    EXPECT_EQ(first->getPath(), "/dir2/match.txt");
    EXPECT_EQ(second->getPath(), "/dir1/match.txt");
}

TEST(FILE_LIBRARY, search_file_max_depth)
{

    auto fl = FileLibrary();
    system(
        "rm -rf /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library ; "
        "mkdir -p /tmp/test_file_library/dir1/dir2/dir3/dir4 ; "
        "touch /tmp/test_file_library/dir1/match.txt ; "
        "touch /tmp/test_file_library/dir1/dir2/match.txt ; "
        "touch /tmp/test_file_library/dir1/dir2/dir3/match.txt ; "
        "touch /tmp/test_file_library/dir1/dir2/dir3/dir4/match.txt ; ");
    fl.addRootFilesystem("/tmp/test_file_library");
    auto results = fl.searchFile("match.txt", 3);
    EXPECT_EQ(results.size(), 3);
    auto first = results.begin();
    auto second = first;
    second++;
    auto third = second;
    third++;
    EXPECT_EQ(first->getPath(), "/dir1/match.txt");
    EXPECT_EQ(second->getPath(), "/dir1/dir2/dir3/match.txt");
    EXPECT_EQ(third->getPath(), "/dir1/dir2/match.txt");
}
