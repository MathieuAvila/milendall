#include "common.hxx"

#include "gltf_model.hxx"
#include "gltf_mesh.hxx"
#include "gltf_animation.hxx"
#include "gltf_data_accessor.hxx"
#include "gltf_exception.hxx"

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
        console->debug("Node name is {}", name);
    }
    /* get all children nodes */
    jsonExecuteAllIfElement(json, "children", [this](nlohmann::json& child, int node_index) {
        console->debug("found child:{}", to_string(child));
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
    auto p = ref.getPath();
    console->info("Load model: {}", p);

    string raw_json;
    bool glb = false;
    FileContentPtr content;

    if (p.size()>4) {
        auto last = (p.substr(p.size() -4, 4));
        if (last == ".glb") { // Not conforming to spec. Don't care.
            glb = true;
            auto contentAll = ref.readContent();

            struct header {
                uint32_t header;
                uint32_t version;
                uint32_t length;

                uint32_t json_length;
                uint32_t json_type;
                char json_data;
            } __attribute__((packed))* head;

            head = (struct header*)&contentAll.get()->at(0);

            console->debug("GLB size={}", contentAll->size());
            console->debug("Header={:x}, version={} length={}",
                        head->header, head->version, head->length);
            console->debug("Json type={:x}, length={}",
                        head->json_type, head->json_length);

            if (head->header != 0x46546C67)
                throw GltfException(std::string("GLB invalid header (should be 0x46546C67)"));
            if (head->version != 2)
                throw GltfException(std::string("GLB invalid version (should be 2)"));
            if (head->json_type != 0x4E4F534A)
                throw GltfException(std::string("GLB invalid JSON type (should be 0x4E4F534A)"));

            char* start = &(head->json_data);
            char* end = start + head->json_length;
            *end = 0;
            raw_json = string(start);

            struct header_data {
                uint32_t length;
                uint32_t type;
                char data;
            } __attribute__((packed))* head_data = (struct header_data*)end;

            console->debug("Data type={:x}, length={}",
                        head_data->type, head_data->length);

            if (head_data->type != 0x004E4942)
                throw GltfException(std::string("GLB invalid data type (should be 0x004E4942)"));

            char* start_data = &(head_data->data) ;
            char* end_data = (char*)&(contentAll->at(contentAll->size()-1))+1 ; // start_data + head_data->length;
            auto table = std::make_shared<std::vector<uint8_t>>();
            table->assign(start_data, end_data);
            content = FileContentPtr(table);
        }
    }
    if (!glb) {
        raw_json = ref.readStringContent();
    }

    nlohmann::json file_json;
    try
    {
        file_json = json::parse(raw_json.c_str());
        //console->info("{}", file_json.dump(1));
    }
    catch (json::parse_error& e)
    {
        console->error("message: {}, exception id: {} byte position of error: {}",
                        e.what() , e.id, e.byte);
        throw GltfException(std::string("GLTF parse error ") + e.what() + " at char: " + std::to_string(e.byte));
    }
    catch (json::exception& e)
    {
        console->error("message: {}, exception id: {}",
                        e.what() , e.id);
        throw GltfException(std::string("GLTF parse error ") + e.what());
    }

    // build a data accessor from this
    unique_ptr<GltfDataAccessorIFace> data_accessor(new GltfDataAccessor(file_json, ref.getDirPath(), content));
    // access a material accessor from the json file
    materialAccessor = materialLibrary->getMaterialAccessor(ref.getDirPath(), file_json);
    // Load all meshes, whether used or not
    jsonExecuteAllIfElement(file_json, "meshes", [this, &data_accessor](nlohmann::json& child, int node_index) {
        console->debug("Load mesh: {}", node_index);
        meshTable.push_back(make_shared<GltfMesh>(child, data_accessor.get(), materialAccessor));
    });

    // Load all nodes, whether used or not
    jsonExecuteAllIfElement(file_json, "nodes", [this, &nodeProvider, &data_accessor](nlohmann::json& child, int node_index) {
        console->debug("Load node: {}", node_index);
        // Force name if none is provided
        if (!child.contains("name") || child["name"].get<std::string>() == "") {
            child["name"] = std::string("#") + std::to_string(node_index);
        }
        nodeTable.push_back(nodeProvider(child, data_accessor.get()));
    });

    // Load all animations, it's up to the application to use them
    jsonExecuteAllIfElement(file_json, "animations", [this, &data_accessor](nlohmann::json& child, int index) {
        console->debug("Load animation: {}", index);
        auto animation = make_shared<GltfAnimation>(child, data_accessor.get());
        animationMap.insert(std::pair<std::string, std::shared_ptr<GltfAnimation>>(animation->getName(), animation));
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
    console->debug("scene ID={} (amongst {}), has children count:{}, root_nodes is:{}",
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

void GltfModel::applyAnimation(GltfInstance* instance, std::string name, float time)
{
    if (!animationMap.count(name))
        throw GltfException("Unknown animation");

    auto anim = animationMap[name];
    anim->apply(time, instance);
}
