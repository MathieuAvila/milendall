#pragma once

#include <nlohmann/json.hpp>
#include <glm/mat4x4.hpp>

#include "file_library.hxx"
#include "gltf_mesh.hxx"

class GltfDataAccessorIFace;
class GltfMaterialAccessorIFace;

struct GltfNode
{
    /** @brief index to mesh */
    int my_mesh;

    /** @brief list of index of children */
    std::vector<unsigned int> children;

    /** @brief default transform matrix */
    glm::mat4x4 default_transform;

    /** @brief instantiate is some kind of direct read
     * @param json pointer to direct JSON of the node
    */
    GltfNode(nlohmann::json& json);
};

/** @brief A GLTF model represents a GLTF file. Application-specific data is read and interpreted
 * through subclassable */
class GltfModel
{
    protected:

        /** @brief to be subclassed in case of application specific data
         * that needs specific handling (i.e: rooms) */
        std::shared_ptr<GltfMesh> instantiateMesh(
            nlohmann::json& json,
            GltfDataAccessorIFace* data_accessor,
            GltfMaterialAccessorIFace* material_accessor);

        /** @brief  To be derived in case there is private data */
        virtual void parseApplicationData(nlohmann::json& json);

        /**
         * @brief  Holds the table of meshes, in file order
         */
        std::vector<std::shared_ptr<GltfMesh>> meshTable;
        /**
         * @brief  Holds the table of NODES, in file order
         */
        std::vector<std::shared_ptr<GltfNode>> nodeTable;

    public:

        GltfModel() = delete;
        GltfModel(const GltfModel&) = delete;

        GltfModel(const FileLibrary::UriReference ref);
        virtual ~GltfModel();
};

