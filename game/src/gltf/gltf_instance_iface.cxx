#include "common.hxx"
#include "gltf_model.hxx"
#include "gltf_mesh.hxx"
#include "gltf_instance_iface.hxx"
#include "gltf_exception.hxx"

static auto console = getConsole("gltf_instance");

GltfInstance::GltfInstance(int node_count)
{
    for (int i=0; i!=node_count; i++)
        nodeInstanceTable.push_back(createNodeInstance());
};

std::unique_ptr<GltfNodeInstanceIface> GltfInstance::createNodeInstance()
{
    return std::make_unique<GltfNodeInstanceIface>();
}

GltfNodeInstanceIface* GltfInstance::getNode(int index)
{
    if ((index >= nodeInstanceTable.size()) || (index < 0))
        throw GltfException("Node" + to_string(index) + " does not exist in instance");
    return nodeInstanceTable[index].get();
}

void GltfInstance::applyChange(int index, glm::mat4x4 matrix)
{
    auto node = getNode(index);
    node->stackNodeMatrix(matrix);
}

const glm::mat4x4& GltfNodeInstanceIface::getNodeMatrix()
{
    return local_matrix;
}

void GltfNodeInstanceIface::setNodeMatrix(const glm::mat4x4& value)
{
     local_matrix = value;
     isInvertedMatrixValid = false;
}

void GltfNodeInstanceIface::stackNodeMatrix(const glm::mat4x4& value)
{
     local_matrix = value * local_matrix;
     isInvertedMatrixValid = false;
}

const glm::mat4x4& GltfNodeInstanceIface::getInvertedNodeMatrix()
{
    if (!isInvertedMatrixValid) {
        inverted_local_matrix = glm::inverse(local_matrix);
        isInvertedMatrixValid = true;
    }
    return inverted_local_matrix;
}
