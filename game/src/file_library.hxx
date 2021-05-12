#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>

/* A raw memory container. To be used with *_ptr for auto mgmt */
class FileContent
{
    public:

    /* Initialize. Ownership is passed to FileContent, content must NOT be released after call.
    */
    FileContent(int _size, void * content) : memory_block(content), size(_size) {};
    ~FileContent() { if (memory_block) free(memory_block);};

    void* memory_block;
    unsigned int size;
};

using FileContentPtr = std::shared_ptr<FileContent>;

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

        /** for Ut only */
        std::string getPath() const;
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
