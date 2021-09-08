#include "common.hxx"
#include "gltf_mesh.hxx"
#include "gltf_exception.hxx"
#include "gltf_data_accessor.hxx"
#include "json_helper_accessor.hxx"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <GL/glew.h>

static auto console = spdlog::stdout_color_mt("gltf_mesh");

static void store_if_contains(int& storage, json& json, string name)
{
    storage = -1;
    if (json.contains(name))
        storage = json[name].get<int>();
}

class GltfMesh::GltfPrimitive {

    GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint elementbuffer;
    unsigned int indicesCount;
    int material;
    SGltfMaterialAccessorIFace material_accessor;

public:

    GltfPrimitive(
        json& j_primitive,
        GltfDataAccessorIFace* data_accessor,
        SGltfMaterialAccessorIFace _material_accessor);
    void draw();
    ~GltfPrimitive();
};

GltfMesh::GltfPrimitive::GltfPrimitive(
        json& j_prim,
        GltfDataAccessorIFace* data_accessor,
        SGltfMaterialAccessorIFace _material_accessor) : material_accessor(_material_accessor)
{
    console->debug("New primitive");
    int indices, mode, NORMAL, POSITION, TEXCOORD_0;
    indicesCount = 0;
    material = -1;
    store_if_contains(indices, j_prim, "indices");
    store_if_contains(mode, j_prim, "mode");
    store_if_contains(material, j_prim, "material");
    auto attributes = j_prim[std::string("attributes")];
    store_if_contains(NORMAL, attributes, "NORMAL");
    store_if_contains(POSITION, attributes, "POSITION");
    store_if_contains(TEXCOORD_0, attributes, "TEXCOORD_0");
    console->info(
        "load primitive indices={}, mode={}, "
        "material={}, NORMAL={}, POSITION={}, TEXCOORD_0={}",
        indices, mode, material, NORMAL, POSITION, TEXCOORD_0);

    if (indices == -1) {
        console->warn("Only indexed mode supported");
        return;
    }
    if (material == -1) {
        console->warn("Only material mode supported");
        return;
    }
    if (POSITION == -1) {
        console->warn("Need POSITION attrib");
        return;
    }
    if (TEXCOORD_0 == -1) {
        console->warn("Need TEXCOORD_0 attrib");
        return;
    }
    if (mode != 4) {
        console->warn("Only mode 4 supported");
        return;
    }
    auto indexed_vertices = data_accessor->accessId(POSITION);
    auto indexed_uvs = data_accessor->accessId(TEXCOORD_0);
    //auto normalbuffer = data_accessor->accessId(NORMAL);
    auto indicesbuffer = data_accessor->accessId(indices);

    glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices->count * sizeof(glm::vec3), indexed_vertices->data, GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs->count * sizeof(glm::vec2), indexed_uvs->data, GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesbuffer->count * sizeof(unsigned short), indicesbuffer->data , GL_STATIC_DRAW);

    indicesCount = indicesbuffer->count;

    console->info(
        "vertexbuffer={}, uvbuffer={}, elementbuffer={}",
        vertexbuffer, uvbuffer, elementbuffer);
}

GltfMesh::GltfMesh(
        json& j_mesh,
        GltfDataAccessorIFace* data_accessor,
        SGltfMaterialAccessorIFace material_accessor)
{
    console->debug("New mesh");
    jsonExecuteAllIfElement(j_mesh, "primitives", [this, data_accessor, &material_accessor](nlohmann::json& j_prim, int node_index) {
        primitives.push_back(make_unique<GltfPrimitive>(j_prim, data_accessor, material_accessor));
    });
    if (j_mesh.contains("name")) {
        auto name = j_mesh["name"].get<string>();
        console->info("Mesh has name {}", name);
    }
}

void GltfMesh::GltfPrimitive::draw()
{
    console->info("Draw prim {}, {}, {}, {}", vertexbuffer, uvbuffer, elementbuffer, indicesCount);
    material_accessor->loadId(material);

    // 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indicesCount,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

GltfMesh::GltfPrimitive::~GltfPrimitive()
{
    console->debug("Delete primitive");
}

GltfMesh::~GltfMesh()
{
}

void GltfMesh::parseApplicationData(json& json)
{
}

void GltfMesh::Draw()
{
    for (auto& prim: primitives) {
        prim->draw();
    }
}

