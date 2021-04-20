#include <string>
#include <vector>
#include <memory>

/* A raw memory container. To be used with *_ptr for auto mgmt */
class FileContent
{
    public:

    /* Initialize. Ownership is passed to FileContent, content must NOT be released after call.
    */
    FileContent(void * content) : memory_block(content) {};
    ~FileContent() { free(memory_block);};

    private:

    void* memory_block;
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

        std::vector<UriReference> list_directory();
        bool is_directory();

        std::shared_ptr<FileContent> read_content();

    };


    /** Initialize with no content */
    FileLibrary();

    /** Append a root filesystem. Take all *.zip files and overlay them. */
    void addRootFilesystem(std::string);

    /** Append a specific zip file and overlay it. */
    void addZipFiles(std::string);

    /* get a raw content */
    std::shared_ptr<FileContent> readContent();

    std::shared_ptr<FileLibrary::UriReference> getRoot();

    private:

    /* TODO */
};
