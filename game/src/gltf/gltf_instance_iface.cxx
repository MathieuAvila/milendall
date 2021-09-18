#include "common.hxx"
#include "gltf_model.hxx"
#include "gltf_mesh.hxx"
#include "gltf_instance_iface.hxx"

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
    return nodeInstanceTable[index].get();
}

const glm::mat4x4& GltfNodeInstanceIface::getNodeMatrix()
{
    return local_matrix;
};

void GltfNodeInstanceIface::setNodeMatrix(const glm::mat4x4& value)
{
     local_matrix = value;
     isInvertedMatrixValid = false;
};

const glm::mat4x4& GltfNodeInstanceIface::getInvertedNodeMatrix()
{
    if (!isInvertedMatrixValid) {
        inverted_local_matrix = glm::inverse(local_matrix);
        isInvertedMatrixValid = true;
    }
    return inverted_local_matrix;
};