#pragma once

#include <memory>

#include "file_library.hxx"
#include "gltf_model.hxx"

class Room
{
    public:

        /** build the room from the directory path */
        Room(GltfMaterialLibraryIfacePtr materialLibrary, FileLibrary::UriReference& ref);

    private:

        std::unique_ptr<GltfModel> model;
};


