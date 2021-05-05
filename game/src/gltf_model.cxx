#include "gltf_model.hxx"
#include "gltf_frame.hxx"
#include "gltf_data_accessor.hxx"

using namespace std;
using namespace nlohmann;

std::shared_ptr<GltfFrame> GltfModel::instantiateFrame(
    nlohmann::json& json,
    GltfDataAccessorIFace* data_accessor,
    GltfMaterialAccessorIFace* material_accessor)
{
    return std::make_shared<GltfFrame>(
        json, 0, data_accessor, material_accessor);
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
    // build a data accessor from this
    unique_ptr<GltfDataAccessorIFace> data_accessor(new GltfDataAccessor(json_element, ref.getDirPath()));
}

GltfModel::~GltfModel()
{
}