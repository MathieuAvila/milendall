#include "common.hxx"
#include "gltf_mesh.hxx"
#include "gltf_data_accessor.hxx"
#include "json_helper_accessor.hxx"

static auto console = spdlog::stdout_color_mt("gltf_mesh");

static void store_if_contains(int& storage, json& json, string name)
{
    storage = -1;
    if (json.contains(name))
        storage = json[name].get<int>();
}

GltfMesh::GltfMesh(
        json& j_mesh,
        GltfDataAccessorIFace* data_accessor,
        GltfMaterialAccessorIFace* material_accessor)
{
    console->debug("New mesh");
    jsonExecuteAllIfElement(j_mesh, "primitives", [this, &j_mesh](nlohmann::json& j_prim, int node_index) {
        int indices, mode, material, NORMAL, POSITION, TEXCOORD_0;
        store_if_contains(indices, j_prim, "indices");
        store_if_contains(mode, j_prim, "mode");
        store_if_contains(material, j_prim, "material");
        auto attributes = j_prim[std::string("attributes")];
        store_if_contains(NORMAL, attributes, "NORMAL");
        store_if_contains(POSITION, attributes, "POSITION");
        store_if_contains(TEXCOORD_0, attributes, "TEXCOORD_0");
        console->debug(
            "load primitive indices={}, mode={}, "
            "material={}, NORMAL={}, POSITION={}, TEXCOORD_0={}",
            indices, mode, material, NORMAL, POSITION, TEXCOORD_0);
    });
    if (j_mesh.contains("name")) {
        auto name = j_mesh["name"].get<string>();
        console->info("Mesh has name {}", name);
    }
}

GltfMesh::~GltfMesh()
{
}

void GltfMesh::parseApplicationData(json& json)
{
}

