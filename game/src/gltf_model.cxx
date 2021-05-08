#include "common.hxx"
#include "gltf_model.hxx"
#include "gltf_frame.hxx"
#include "gltf_data_accessor.hxx"
#include "gltf_table_accessor.hxx"

static auto console = spdlog::stdout_color_mt("gltf_model");

shared_ptr<GltfFrame> GltfModel::instantiateFrame(
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
    console->info("Load level: {}", ref.getPath());
    auto raw_json = ref.readStringContent();
    auto file_json = json::parse(raw_json.c_str());
    // build a data accessor from this
    unique_ptr<GltfDataAccessorIFace> data_accessor(new GltfDataAccessor(file_json, ref.getDirPath()));
    auto frame_nr = jsonGetElementByName(file_json, "nodes").size();
    console->info("Frame count: {}", frame_nr);
    for(int frame_index = 0; frame_index < frame_nr; frame_index++) {
        frameTable.push_back(std::make_shared<GltfFrame>(
            file_json, frame_index, data_accessor.get(), nullptr));
    }
}

GltfModel::~GltfModel()
{
}