#include <string>
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
        UriReference();

        std::list<UriReference> listDirectory();
        bool is_directory();

        std::shared_ptr<FileContent> readContent();

        /** from current position, get an object to a sub-path.
         * If this starts with "/", original path is removed
         */
        UriReference getSubPath(std::string);

        /** for Ut only */
        std::string getPath();
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
