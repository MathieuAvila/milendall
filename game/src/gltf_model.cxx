#include "gltf_model.hxx"
#include "gltf_frame.hxx"

std::shared_ptr<GltfFrame> GltfModel::instantiateFrame(
    nlohmann::json& json,
    GltfDataAccessorIFace* data_accessor,
    GltfMaterialAccessorIFace* material_accessor)
{
    return std::make_shared<GltfFrame>(json,
        nullptr, nullptr);
}

void GltfModel::parseApplicationData(nlohmann::json& json)
{
    // do nothing
}

GltfModel::GltfModel(FileLibrary::UriReference& ref)
{

}
