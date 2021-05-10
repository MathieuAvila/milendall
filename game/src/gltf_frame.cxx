#include "common.hxx"
#include "gltf_frame.hxx"
#include "gltf_data_accessor.hxx"
#include "gltf_table_accessor.hxx"

static auto console = spdlog::stdout_color_mt("gltf_frame");

static void store_if_contains(int& storage, json& json, string name)
{
    storage = -1;
    if (json.contains(name))
        storage = json[name].get<int>();
}

GltfMesh::GltfMesh(
        json& json,
        int my_mesh,
        GltfDataAccessorIFace* data_accessor,
        GltfMaterialAccessorIFace* material_accessor)
{
    console->debug("Mesh NR={}, has mesh {}", my_mesh, my_mesh);
    auto j_mesh = jsonGetElementByIndex(json, "meshes", my_mesh);
    console->debug("Mesh NR={}, mesh has primitives nr={}", my_mesh, j_mesh.size());
    auto j_prims = jsonGetElementByName(j_mesh, "primitives");
    for(auto j_prim : j_prims) {
        int indices, mode, material, NORMAL, POSITION, TEXCOORD_0;
        store_if_contains(indices, j_prim, "indices");
        store_if_contains(mode, j_prim, "mode");
        store_if_contains(material, j_prim, "material");
        auto attributes = j_prim[std::string("attributes")];
        store_if_contains(NORMAL, attributes, "NORMAL");
        store_if_contains(POSITION, attributes, "POSITION");
        store_if_contains(TEXCOORD_0, attributes, "TEXCOORD_0");
        console->debug(
            "Mesh NR={}, load primitive indices={}, mode={}, "
            "material={}, NORMAL={}, POSITION={}, TEXCOORD_0={}",
            my_mesh, indices, mode, material, NORMAL, POSITION, TEXCOORD_0);
    }
    if (j_mesh.contains("name")) {
        auto name = j_mesh["name"].get<string>();
        console->info("Mesh NR={}, has name {}", my_mesh, name);
    }
}

GltfMesh::~GltfMesh()
{
}

void GltfMesh::parseApplicationData(json& json)
{
}

