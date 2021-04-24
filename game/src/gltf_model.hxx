#ifndef GLTF_MODEL_HXX
#define GLTF_MODEL_HXX

#include <nlohmann/json.hpp>

#include "file_library.hxx"
#include "gltf_frame.hxx"

class GltfModel
{
    protected:

        /** to be subclassed in case of application specific data
         * that needs specific handling (i.e: rooms) */
        std::shared_ptr<GltfFrame> instantiateFrame(nlohmann::json& json);

        /** To be derived in case there is private data */
        virtual void parseApplicationData(nlohmann::json& json);

        /**
         * Holds the table of objects, in file order for easy retrieval of parent/children
         * relationships
         */
        std::vector<std::shared_ptr<GltfFrame>> frameTable;

    public:

        GltfModel(FileLibrary::UriReference& ref);

};

#endif