#include "gltf_model.hxx"
#include "gltf_frame.hxx"

using namespace std;
using namespace nlohmann;

std::shared_ptr<GltfFrame> GltfModel::instantiateFrame(
    nlohmann::json& json,
    GltfDataAccessorIFace* data_accessor,
    GltfMaterialAccessorIFace* material_accessor)
{
    return std::make_shared<GltfFrame>(
        json, 0, nullptr, nullptr);
    //return std::shared_ptr<GltfFrame>(nullptr);
}

void GltfModel::parseApplicationData(nlohmann::json& json)
{
    // do nothing
}

GltfModel::GltfModel(const FileLibrary::UriReference ref)
{
    auto raw_json = ref.readStringContent();
    auto json_element = json::parse(raw_json.c_str());
}

GltfModel::~GltfModel()
{
}