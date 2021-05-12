#include "common.hxx"
#include "gltf_model.hxx"
#include "gltf_mesh.hxx"
#include "gltf_data_accessor.hxx"
#include "json_helper_accessor.hxx"
#include "json_helper_math.hxx"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "helper_math.hxx"

static auto console = spdlog::stdout_color_mt("gltf_model");

static glm::mat4x4 identity(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
        );

GltfNode::GltfNode(json& json): default_transform(identity)
{
    my_mesh = -1;
    /* check it has an associated mesh */
    if (json.contains("mesh")) {
        my_mesh = jsonGetElementByName(json, "mesh").get<int>();
    }
    /* get all children nodes */
    jsonExecuteAllIfElement(json, "children", [this](nlohmann::json& child, int node_index) {
        console->info("found child:{}", to_string(child));
        children.push_back(child.get<int>());
    });
    /* read default raw matrix */
    jsonExecuteIfElement(json, "matrix", [this](nlohmann::json& child) {
        default_transform = jsonGetMatrix4x4(child);
    });

    // TODO : T*R*S is probably not implemented correctly and not tested.

    /* read matrix for scale */
    jsonExecuteIfElement(json, "scale", [this](nlohmann::json& child) {
        default_transform = glm::scale(default_transform, jsonGetVec3(child));
    });
    /* read matrix for rotation */
    jsonExecuteIfElement(json, "rotation", [this](nlohmann::json& child) {
        auto xyzw = jsonGetVec4(child);
        auto quat = glm::qua(xyzw[3], xyzw[0], xyzw[1], xyzw[2]);
        default_transform = default_transform * toMat4(quat);
    });

    /* read matrix for translation */
    jsonExecuteIfElement(json, "translation", [this](nlohmann::json& child) {
        default_transform = glm::translate(default_transform, jsonGetVec3(child));
    });

}

shared_ptr<GltfMesh> GltfModel::instantiateMesh(
    nlohmann::json& json,
    GltfDataAccessorIFace* data_accessor,
    GltfMaterialAccessorIFace* material_accessor)
{
    return make_shared<GltfMesh>(json, data_accessor, material_accessor);
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

    // Load all meshes, whether used or not
    jsonExecuteAllIfElement(file_json, "meshes", [this, &data_accessor](nlohmann::json& child, int node_index) {
        console->info("Load mesh: {}", node_index);
        meshTable.push_back(instantiateMesh(child, data_accessor.get(), nullptr));
    });

    // Load all nodes, whether used or not
    jsonExecuteAllIfElement(file_json, "nodes", [this](nlohmann::json& child, int node_index) {
        console->debug("Load node: {}", node_index);
        nodeTable.push_back(make_shared<GltfNode>(child));
    });

    auto my_scene = jsonGetElementByName(file_json, "scene").get<int>();
    auto j_scenes = jsonGetElementByName(file_json, "scenes");
    auto j_scenes_0 = jsonGetElementByIndex(file_json, "scenes", 0);
    auto scene_child_count = j_scenes_0.size();
    console->info("scene ID={} (amongst {}), has children count:{}", my_scene, j_scenes.size(), scene_child_count);
}

GltfModel::~GltfModel()
{
}