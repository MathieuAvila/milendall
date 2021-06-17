#include "common.hxx"
#include "gltf_model.hxx"
#include "gltf_mesh.hxx"
#include "gltf_instance_iface.hxx"

static auto console = spdlog::stdout_color_mt("gltf_instance");

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