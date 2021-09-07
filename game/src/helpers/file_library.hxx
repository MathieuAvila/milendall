#pragma once

#include <string>
#include <vector>
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

using FileContentPtr = std::unique_ptr< std::vector<uint8_t> >;

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
        UriReference(const UriReference&) = default;
        UriReference();

        std::vector<UriReference> listDirectory() const;
        bool is_directory() const;

        /** read a binary content */
        FileContentPtr readContent() const;

        /** read any content that looks like a string, i.e: a JSON file */
        std::string readStringContent() const;

        /** from current position, get an object to a sub-path.
         * If this starts with "/", original path is removed
         */
        UriReference getSubPath(std::string) const;

        /** from current position, get an object to the original directory.
         * basically: remove last path element
         */
        UriReference getDirPath() const;

        /** for UT and debug only */
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

    private:

    std::list<std::string> root_list;

};

using FileLibraryPtr = std::shared_ptr<FileLibrary>;
