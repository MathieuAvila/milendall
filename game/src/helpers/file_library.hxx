#pragma once

#include <string>
#include <list>
#include <vector>
#include <memory>
#include <stdexcept>

/** @brief A single GLTF exception when a loading errors happens */
class LibraryException: public std::runtime_error
{
    public:

        LibraryException(const std::string err): runtime_error(err) {};
};

using FileContentPtr = std::shared_ptr< std::vector<uint8_t> >;

/** An abstraction of filesystem, that masks the fact that things are
 * in real FS roots or embedded zipped files.
 * ZIP files are expected to have a namespaced root directory to avoid collision.
 * Prevalence is determined by order, first registered have priority, to avoid
 * illegetimate crushing */
class FileLibrary
{
    public:

    class UriReference
    {
        FileLibrary* master;
        std::string path;

        public:

        UriReference(FileLibrary*, std::string);
        UriReference(UriReference&& ref);
        UriReference operator=(UriReference ref);
        UriReference(const UriReference&) = default;
        bool operator<(UriReference& ref);
        UriReference();

        std::vector<UriReference> listDirectory() const;

        /** @brief read a binary content */
        FileContentPtr readContent() const;

        /** @brief read any content that looks like a string, i.e: a JSON file */
        std::string readStringContent() const;

        /** @brief from current position, get an object to a sub-path.
         * If this starts with "/", original path is removed
         */
        UriReference getSubPath(std::string) const;

        /** @brief from current position, get an object to the original directory.
         * basically: remove last path element
         */
        UriReference getDirPath() const;

        /** @brief get filename. This is the last element of the path
         */
        std::string getFileName() const;

        /** @brief Check there is a file present with this reference */
        bool isFile() const;

        /** @brief Check there is a directory present with this reference */
        bool isDirectory() const;

        /** @brief for UT and debug only */
        std::string getPath() const;

        bool operator<(const FileLibrary::UriReference& r ) const;
    };


    /** Initialize with no content */
    FileLibrary();

    /** Append a root filesystem. Take all *.zip files and overlay them. */
    void addRootFilesystem(std::string);

    /** Append a specific zip file and overlay it. */
    void addZipFiles(std::string);

    /* get an instance to root */
    FileLibrary::UriReference getRoot();

    /** @brief Search for a list of files with a given name (Used to seek level.json files)
     * @param file_name The file name to search
     * @param depth max dir depth
    */
    std::vector<FileLibrary::UriReference> searchFile(std::string file_name, int depth = 3);

    private:

    std::list<std::string> root_list;

    /** @brief recurse function to search for file
     */
    void recurseSearchFile(
        std::string file_name,
        std::vector<FileLibrary::UriReference>& results,
        FileLibrary::UriReference& dir,
        int inverse_depth);
};

using FileLibraryPtr = std::shared_ptr<FileLibrary>;
