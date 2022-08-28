#include "common.hxx"
#include "gltf_mesh.hxx"
#include "gltf_exception.hxx"
#include "gltf_data_accessor.hxx"
#include "json_helper_accessor.hxx"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <GL/glew.h>

static auto console = getConsole("gltf_mesh");

static void store_if_contains(int& storage, json& json, string name)
{
    storage = -1;
    if (json.contains(name))
        storage = json[name].get<int>();
}

class GltfMesh::GltfPrimitive {

    struct bufferDef {
        GLuint buffer;
        unsigned stride;
        int glformat;
    };
    struct bufferDef vertexbuffer;
	struct bufferDef uvbuffer;
	struct bufferDef elementbuffer;
    struct bufferDef normalbuffer;
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
    NORMAL = -1;
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
    std::unique_ptr<GltfDataAccessorIFace::DataBlock> indexed_normal;
    if (NORMAL != -1) {
        indexed_normal = data_accessor->accessId(NORMAL);
    }
    auto indicesbuffer = data_accessor->accessId(indices);

    vertexbuffer.stride = indexed_vertices->stride;
    uvbuffer.stride = indexed_uvs->stride;
    elementbuffer.stride = indicesbuffer->stride;

    std::map<GltfDataAccessor::DataBlock::UNIT_TYPE, unsigned int> mapper_gl_type = {
           {GltfDataAccessor::DataBlock::FLOAT,          GL_FLOAT },
           {GltfDataAccessor::DataBlock::UNSIGNED_INT,   GL_UNSIGNED_INT},
           {GltfDataAccessor::DataBlock::UNSIGNED_SHORT, GL_UNSIGNED_SHORT}
    };

    std::map<GltfDataAccessor::DataBlock::UNIT_TYPE, unsigned long> mapper_gl_size = {
           {GltfDataAccessor::DataBlock::FLOAT,          8 },
           {GltfDataAccessor::DataBlock::UNSIGNED_INT,   4 },
           {GltfDataAccessor::DataBlock::UNSIGNED_SHORT, 2 }
    };

    vertexbuffer.glformat = mapper_gl_type[indexed_vertices->unit_type];
    uvbuffer.glformat = mapper_gl_type[indexed_uvs->unit_type];
    elementbuffer.glformat = mapper_gl_type[indicesbuffer->unit_type];

    glGenBuffers(1, &vertexbuffer.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer.buffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices->count * sizeof(glm::vec3), indexed_vertices->data, GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer.buffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs->count * sizeof(glm::vec2), indexed_uvs->data, GL_STATIC_DRAW);

    normalbuffer.buffer = 0;
    if (NORMAL != -1) {
        normalbuffer.glformat = mapper_gl_type[indexed_normal->unit_type];
        glGenBuffers(1, &normalbuffer.buffer);
	    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer.buffer);
	    glBufferData(GL_ARRAY_BUFFER, indexed_normal->count * sizeof(glm::vec3), indexed_normal->data, GL_STATIC_DRAW);
    }

    glGenBuffers(1, &elementbuffer.buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer.buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned long)(indicesbuffer->count) * mapper_gl_size[indicesbuffer->unit_type], indicesbuffer->data , GL_STATIC_DRAW);

    indicesCount = indicesbuffer->count;


    console->debug(
        "vertexbuffer={} - stride={} - format={}",
        vertexbuffer.buffer,vertexbuffer.stride, vertexbuffer.glformat);
    console->debug(
        "uvbuffer={} - stride={} - format={}",
        uvbuffer.buffer,uvbuffer.stride, uvbuffer.glformat);
    console->debug(
        "elementbuffer={} - stride={} - format={}",
        elementbuffer.buffer,elementbuffer.stride, elementbuffer.glformat);
    console->debug(
        "normalbuffer={} - stride={} - format={}",
        normalbuffer.buffer,normalbuffer.stride, normalbuffer.glformat);
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
        console->debug("Mesh has name {}", name);
    }
}

void GltfMesh::GltfPrimitive::draw()
{
    //console->info("Draw prim {}, {}, {}, {}", vertexbuffer, uvbuffer, elementbuffer, indicesCount);
    material_accessor->loadId(material);

    // 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer.buffer);
	glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			vertexbuffer.glformat,           // type
			GL_FALSE,           // normalized?
			vertexbuffer.stride,// stride
			(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs or NORMAL
	glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer.buffer);
	glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			uvbuffer.glformat,                // type
			GL_FALSE,                         // normalized?
			uvbuffer.stride,                  // stride
			(void*)0                          // array buffer offset
		);
    if (normalbuffer.buffer) {
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer.buffer);
	    glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			normalbuffer.glformat,            // type
			GL_FALSE,                         // normalized?
			normalbuffer.stride,              // stride
			(void*)0                          // array buffer offset
		);
    }

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer.buffer);

		// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indicesCount,    // count
		elementbuffer.glformat,   // type
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

