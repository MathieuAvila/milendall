#pragma once

#include <vector>
#include <memory>
#include <glm/mat4x4.hpp>

class GltfNodeInstanceIface
{
    protected:

    glm::mat4x4 local_matrix = glm::mat4(1.0);

    bool isInvertedMatrixValid;
    glm::mat4x4 inverted_local_matrix = glm::mat4(1.0);

    public:

    /**
     * Accessor to local matrix
     */
    const glm::mat4x4& getNodeMatrix();
    void setNodeMatrix(const glm::mat4x4&);

     /**
     * Accessor to inverted local matrix
     */
    const glm::mat4x4& getInvertedNodeMatrix();

    /** default instantiate */
    GltfNodeInstanceIface() = default;

    /** Needed in case of subclassing for private data */
    virtual ~GltfNodeInstanceIface() = default;
};

/** @brief A GLTF instance represents a particular instance of a GLTF object.
*/
class GltfInstance
{
    protected:

    /**
    * @brief  Holds the table of nodes instances, same order as original version
    */
    std::vector<std::unique_ptr<GltfNodeInstanceIface>> nodeInstanceTable;

    public:

    /** Automatically set-up the list of nodes based on node count */
    GltfInstance(int node_count);

    /** Builder pattern: make it virtual so that it can be extended. */
    virtual std::unique_ptr<GltfNodeInstanceIface> createNodeInstance();

    /** get a sub-node. This is the way the model traverses the nodes */
    GltfNodeInstanceIface* getNode(int);

    virtual ~GltfInstance() = default;
};

