#include "common.hxx"
#include "gltf_model.hxx"
#include "gltf_frame.hxx"
#include "gltf_data_accessor.hxx"
#include "gltf_table_accessor.hxx"

static auto console = spdlog::stdout_color_mt("gltf_model");

GltfNode::GltfNode(json& json)
{

}

shared_ptr<GltfMesh> GltfModel::instantiateFrame(
    nlohmann::json& json,
    GltfDataAccessorIFace* data_accessor,
    GltfMaterialAccessorIFace* material_accessor)
{
    return std::make_shared<GltfMesh>(
        json, 0, data_accessor, material_accessor);
    //return std::shared_ptr<GltfMesh>(nullptr);
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
    auto node_nr = jsonGetElementByName(file_json, "nodes").size();
    console->info("Nodes count: {}", node_nr);
    // Load all nodes, whether used or not
    for(int node_index = 0; node_index < node_nr; node_index++) {
        console->info("Load node with index: {}", node_index);
        auto j_node = jsonGetElementByIndex(file_json, "nodes", node_index);
        /* check it has an associated mesh */
        if (j_node.contains("mesh")) {
            auto my_mesh = jsonGetElementByName(j_node, "mesh").get<int>();
        }
    }
    // Load all meshes, whether used or not
    auto mesh_nr = jsonGetElementByName(file_json, "meshes").size();
    console->info("Meshes count: {}", mesh_nr);
    for(int mesh_index = 0; mesh_index < mesh_nr; mesh_index++) {

        meshTable.push_back(std::make_shared<GltfMesh>(
            file_json, mesh_index, data_accessor.get(), nullptr));
    }

    auto my_scene = jsonGetElementByName(file_json, "scene").get<int>();
    auto j_scenes = jsonGetElementByName(file_json, "scenes");
    auto j_scenes_0 = jsonGetElementByIndex(file_json, "scenes", 0);
    auto scene_child_count = j_scenes_0.size();
    console->info("scene ID={} (amongst {}), has children count:{}", my_scene, j_scenes.size(), scene_child_count);
}

GltfModel::~GltfModel()
{
}