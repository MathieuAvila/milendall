#pragma once

#include <nlohmann/json.hpp>
#include <glm/mat4x4.hpp>

#include "file_library.hxx"
#include "gltf_mesh.hxx"

#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_instance_iface.hxx"

class GltfDataAccessorIFace;
class GltfInstance;

struct GltfNode
{
    typedef std::vector<unsigned int> ChildrenList;

    /** @brief Name from the GLTF. For debug */
    std::string name;

    /** @brief index to mesh */
    int my_mesh;

    /** @brief list of index of children */
    ChildrenList children;

    /** @brief default transform matrix */
    glm::mat4x4 default_transform;

    /** @brief instantiate is some kind of direct read
     * @param json pointer to direct JSON of the node
    */
    GltfNode(nlohmann::json& json);
    virtual ~GltfNode() = default;
};

using GltfNodeProvider = std::shared_ptr<GltfNode>(*) (nlohmann::json& json, GltfDataAccessorIFace* data_accessor);

/** @brief A GLTF model represents a GLTF file. Application-specific data is read and interpreted
 * through subclassable */
class GltfModel
{
    protected:

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

        /** Holds the root node that is the Father of Them All */
        std::vector<int> root_nodes;

        SGltfMaterialAccessorIFace materialAccessor;

        /** recursive method to draw things */
        void draw(GltfInstance*, int);

        /** recursive method to apply default transform */
        void applyDefaultTransform(GltfInstance* instance, int index, glm::mat4& position);

    public:

        GltfModel() = delete;
        GltfModel(const GltfModel&) = delete;

        GltfModel(GltfMaterialLibraryIfacePtr materialLibrary, const FileLibrary::UriReference ref,
                    GltfNodeProvider nodeProvider);
        GltfModel(GltfMaterialLibraryIfacePtr materialLibrary, const FileLibrary::UriReference ref);
        virtual ~GltfModel();

        /** Get node count, in order to create instance */
        int getInstanceParameters();

        /** Set a default transformation in an instance, no animation */
        void applyDefaultTransform(GltfInstance* instance, glm::mat4& position);

        /** Draw an instance, using matrices from it */
        void draw(GltfInstance* instance);
};

