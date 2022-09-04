#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

#include "common.hxx"
#include "file_library.hxx"

static auto console = getConsole("library");

std::string FileLibrary::UriReference::getPath() const
{
    return path;
}

FileLibrary::UriReference::UriReference(FileLibrary::UriReference &&org)
{
    path = org.path;
    master = org.master;
}

FileLibrary::UriReference::UriReference(FileLibrary *fl, std::string _path)
{
    path = std::filesystem::path(_path).lexically_normal();
    // unless path is "/", remove trailing "/" if any
    if ((path.length() > 1) && (path[path.length() - 1] == '/'))
        path = path.substr(0, path.length() - 1);
    master = fl;
    console->debug("Create path: {}", path);
}

FileLibrary::UriReference FileLibrary::UriReference::getDirPath() const
{
    auto ppath = std::filesystem::path(path).parent_path();
    return UriReference(master, ppath);
}

std::string FileLibrary::UriReference::getFileName() const
{
    return std::filesystem::path(path).filename();
}

std::vector<FileLibrary::UriReference> FileLibrary::UriReference::listDirectory() const
{
    std::vector<FileLibrary::UriReference> result;
    for (auto dir_path : master->root_list)
    {
        for (const auto &entry : std::filesystem::directory_iterator(dir_path + "/" + path))
        {
            std::string file_path = entry.path();
            auto final_path = file_path.substr(dir_path.length());
            result.push_back(UriReference(master, final_path));
        }
    }
    /** todo: add ZIP contents */
    return result;
}

FileLibrary::UriReference FileLibrary::UriReference::getSubPath(std::string sub_path) const
{
    if ((sub_path.length() > 0) && (sub_path[0] == '/'))
        return UriReference(master, sub_path);
    return FileLibrary::UriReference(master, path + "/" + sub_path);
}

bool FileLibrary::UriReference::isDirectory() const
{
    for (auto dir_path : master->root_list)
    {
        // Get first filename that matches requested name
        auto final_path = dir_path + path;
        if (std::filesystem::exists(final_path))
        {
            return std::filesystem::is_directory(final_path);
        }
    }
    return false;
}

bool FileLibrary::UriReference::isFile() const
{
    for (auto dir_path : master->root_list)
    {
        // Get first filename that matches requested name
        auto final_path = dir_path + path;
        if (std::filesystem::exists(final_path))
        {
            return std::filesystem::is_regular_file(final_path);
        }
    }
    return false;
}

FileContentPtr FileLibrary::UriReference::readContent() const
{
    console->debug("Tentatively open file", path);
    for (auto dir_path : master->root_list)
    {
        // Get first filename that matches requested name
        auto final_path = dir_path + path;
        console->debug("Check if file {} is a match", final_path);
        if (std::filesystem::is_regular_file(final_path))
        {
            console->debug("Is a match, now try to open", final_path);
            auto file_size = std::filesystem::file_size(final_path);
            FILE *file = fopen(final_path.c_str(), "rb");
            FileContentPtr result = make_shared<std::vector<uint8_t>>(file_size);
            fread(result->data(), file_size, 1, file);
            return result;
        }
    }
    console->error("file not found: {}", path);
    throw LibraryException(string("file not found: {}") + path);
}

std::string FileLibrary::UriReference::readStringContent() const
{
    console->debug("Tentatively open file for string", path);
    auto content = readContent();
    return string(content->data(), content->data() + content->size());
}

bool FileLibrary::UriReference::operator<(const FileLibrary::UriReference &r) const
{
    return path < r.path;
}

FileLibrary::FileLibrary(){};

void FileLibrary::addRootFilesystem(std::string path)
{
    root_list.push_back(path);
};

void FileLibrary::addZipFiles(std::string){};

FileLibrary::UriReference FileLibrary::getRoot()
{
    return FileLibrary::UriReference(this, "/");
};

std::list<FileLibrary::UriReference> FileLibrary::searchFile(std::string file_name, int depth)
{
    std::list<FileLibrary::UriReference> results;
    FileLibrary::UriReference dir = getRoot();
    recurseSearchFile(file_name, results, dir, depth);
    return results;
}

void FileLibrary::recurseSearchFile(
    std::string file_name,
    std::list<FileLibrary::UriReference> &results,
    FileLibrary::UriReference &dir,
    int inverse_depth)
{
    std::vector<FileLibrary::UriReference> all = dir.listDirectory();
    for (auto subfile : all)
    {
        if (subfile.isDirectory())
        {
            if (inverse_depth)
            {
                recurseSearchFile(file_name, results, subfile, inverse_depth - 1);
            }
        }
        else if (subfile.isFile())
        {
            if (subfile.getFileName() == file_name)
                results.push_back(subfile);
        }
    }
}