#include "common.hxx"
#include "gltf_frame.hxx"
#include "gltf_data_accessor.hxx"
#include "gltf_table_accessor.hxx"

static auto console = spdlog::stdout_color_mt("gltf_frame");

GltfFrame::GltfFrame(
        json& json,
        int index,
        GltfDataAccessorIFace* data_accessor,
        GltfMaterialAccessorIFace* material_accessor)
{
    console->info("Load frame with index: {}", index);
    auto j_frame = jsonGetElementByIndex(json, "", index);
    if (j_frame.contains("mesh")) {
        auto my_mesh = jsonGetElementByName(j_frame, "mesh").get<int>();
        console->info("Frame ID={}, has mesh {}", index, my_mesh);
        auto j_mesh = jsonGetElementByIndex(json, "meshes", my_mesh);
        console->info("Frame ID={}, mesh has primitives nr={}", index, j_mesh.size());
        for(auto j_prim : j_mesh) {
            console->info("Frame ID={}, load primitive", index);
        /*frameTable.push_back(std::make_shared<GltfFrame>(
            file_json, frame_index, data_accessor.get(), nullptr));*/
        }
    }
}

GltfFrame::~GltfFrame()
{
}

void GltfFrame::parseApplicationData(json& json)
{
}

