#include "file_library.hxx"

FileLibrary::UriReference::UriReference(FileLibrary*, std::string)
{

}

std::vector<FileLibrary::UriReference> FileLibrary::UriReference::list_directory()
{

}

bool FileLibrary::UriReference::is_directory()
{

}

std::shared_ptr<FileContent> FileLibrary::UriReference::read_content()
{

}

FileLibrary::FileLibrary()
{
};

void FileLibrary::addRootFilesystem(std::string)
{
};

void FileLibrary::addZipFiles(std::string)
{
};

std::shared_ptr<FileContent> FileLibrary::readContent()
{
};

std::shared_ptr<FileLibrary::UriReference> FileLibrary::getRoot()
{
   return std::make_shared<FileLibrary::UriReference>(this, "/");
};
