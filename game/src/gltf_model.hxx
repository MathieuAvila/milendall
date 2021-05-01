#ifndef GLTF_MODEL_HXX
#define GLTF_MODEL_HXX

#include <nlohmann/json.hpp>

#include "file_library.hxx"
#include "gltf_frame.hxx"

class GltfDataAccessorIFace;
class GltfMaterialAccessorIFace;


/** @brief A GLTF model represents a GLTF file. Application-specific data is read and interpreted
 * through subclassable */
class GltfModel
{
    protected:

        /** @brief to be subclassed in case of application specific data
         * that needs specific handling (i.e: rooms) */
        std::shared_ptr<GltfFrame> instantiateFrame(
            nlohmann::json& json,
            GltfDataAccessorIFace* data_accessor,
            GltfMaterialAccessorIFace* material_accessor);

        /** @brief  To be derived in case there is private data */
        virtual void parseApplicationData(nlohmann::json& json);

        /**
         * @brief  Holds the table of objects, in file order for easy retrieval of parent/children
         * relationships
         */
        std::vector<std::shared_ptr<GltfFrame>> frameTable;

    public:

        GltfModel() = delete;
        GltfModel(const FileLibrary::UriReference ref);
        virtual ~GltfModel();
};

#endif