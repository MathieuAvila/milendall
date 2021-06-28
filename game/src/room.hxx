#pragma once

#include <memory>

#include "file_library.hxx"
#include "gltf_model.hxx"

class Room : public GltfModel
{
    protected:

        virtual void parseApplicationData(nlohmann::json& json);

    public:

        /** build the room from the directory path */
        Room(GltfMaterialLibraryIfacePtr materialLibrary, FileLibrary::UriReference& ref);

};


