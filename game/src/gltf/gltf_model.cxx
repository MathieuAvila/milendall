#include "common.hxx"
#include "gltf_model.hxx"
#include "gltf_mesh.hxx"
#include "gltf_data_accessor.hxx"
#include "json_helper_accessor.hxx"
#include "json_helper_math.hxx"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "helper_math.hxx"

#include "gl_init.hxx"

static auto console = getConsole("gltf_model");

static glm::mat4x4 identity = glm::mat4(1.0f);

GltfNode::GltfNode(json& json): default_transform(identity)
{
    my_mesh = -1;
    /* check it has an associated mesh */
    if (json.contains("mesh")) {
        my_mesh = jsonGetElementByName(json, "mesh").get<int>();
    }
     if (json.contains("name")) {
        name = jsonGetElementByName(json, "name").get<string>();
        console->info("Node name is {}", name);
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

void GltfModel::parseApplicationData(nlohmann::json& json)
{
    // do nothing
}

std::shared_ptr<GltfNode> gltfInstantiateNode(nlohmann::json& json, GltfDataAccessorIFace* data_accessor)
{
    return make_shared<GltfNode>(json);
}

GltfModel::GltfModel(GltfMaterialLibraryIfacePtr materialLibrary, const FileLibrary::UriReference ref):
 GltfModel(materialLibrary, ref, gltfInstantiateNode)
 {
}

GltfModel::GltfModel(GltfMaterialLibraryIfacePtr materialLibrary, const FileLibrary::UriReference ref,
                    GltfNodeProvider nodeProvider)
{
    console->info("Load level: {}", ref.getPath());
    auto raw_json = ref.readStringContent();
    auto file_json = json::parse(raw_json.c_str());
    // build a data accessor from this
    unique_ptr<GltfDataAccessorIFace> data_accessor(new GltfDataAccessor(file_json, ref.getDirPath()));

    // access a material accessor from the json file
    materialAccessor = materialLibrary->getMaterialAccessor(ref.getDirPath(), file_json);

    // Load all meshes, whether used or not
    jsonExecuteAllIfElement(file_json, "meshes", [this, &data_accessor](nlohmann::json& child, int node_index) {
        console->info("Load mesh: {}", node_index);
        meshTable.push_back(make_shared<GltfMesh>(child, data_accessor.get(), materialAccessor));
    });

    // Load all nodes, whether used or not
    jsonExecuteAllIfElement(file_json, "nodes", [this, &nodeProvider, &data_accessor](nlohmann::json& child, int node_index) {
        console->info("Load node: {}", node_index);
        nodeTable.push_back(nodeProvider(child, data_accessor.get()));
    });

    auto my_scene = jsonGetElementByName(file_json, "scene").get<int>();
    auto j_scenes = jsonGetElementByName(file_json, "scenes");
    auto j_scenes_i = jsonGetElementByIndex(file_json, "scenes", my_scene);
    auto scene_child_count = j_scenes_i.size();
    auto j_scenes_i_nodes = jsonGetElementByName(j_scenes_i, "nodes");

    std::string output;
    for (auto node: j_scenes_i_nodes ) {
        auto i = node.get<int>();
        root_nodes.push_back(i);
        output += to_string(i) + " ";
    }
    console->info("scene ID={} (amongst {}), has children count:{}, root_nodes is:{}",
    my_scene, j_scenes.size(), scene_child_count, output);
}

GltfModel::~GltfModel()
{
}

int GltfModel::getInstanceParameters()
{
    return nodeTable.size();
}

void GltfModel::draw(GltfInstance* instance, int index, void* context)
{
    auto myNode = nodeTable[index].get();
    auto instanceNode = instance->getNode(index);
    if (myNode->my_mesh != -1)
    {
		setMeshMatrix(instanceNode->getNodeMatrix());
        meshTable[myNode->my_mesh]->Draw();
    }
    for (auto child_node: myNode->children) {
        draw(instance, child_node, context);
    }
}

void GltfModel::draw(GltfInstance* instance, void* context)
{
    for (auto i: root_nodes)
        draw(instance, i, context);
}

void GltfModel::applyDefaultTransform(GltfInstance* instance, int index, glm::mat4& position)
{
    auto myNode = nodeTable[index].get();
    auto instanceNode = instance->getNode(index);

    auto mat = instanceNode->getNodeMatrix();
    mat = position * myNode->default_transform;
    instanceNode->setNodeMatrix(mat);

    for (int i : myNode->children)
        applyDefaultTransform(instance, i, mat);
}

void GltfModel::applyDefaultTransform(GltfInstance* instance, glm::mat4& position)
{
    for (auto i: root_nodes) {
        applyDefaultTransform(instance, i, position);
    }
}