#include "common.hxx"

#include <GL/glew.h>

#include <stdint.h>

#include "gl_init.hxx"
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include "test_common.hxx"

float currentMetallicFactor;
float currentRoughnessFactor;
unsigned int currentTexture;
float currentColor[3];
bool isCurrentModeTexture;

float mat4x4_abs_diff(const glm::mat4x4& mat1, const glm::mat4x4& mat2)
{
	float result = 0.0f;
	for(glm::length_t i = 0; i < 4; ++i)
	    for(glm::length_t j = 0; j < 4; ++j)
		result += abs(mat1[i][j] - mat2[i][j]);
	return result;
}

bool check_equal_vec3(const glm::vec3 v1, const glm::vec3 v2)
{
    auto epsi = glm::epsilonEqual(v1, v2, 0.1f);
    bool result = (glm::bvec3(true,true,true) == epsi);
    return result;
}

void setMeshMatrix(glm::mat4 mat) {};
void setViewMatrix(glm::mat4x4 mat, glm::vec3 pos) {};
int getValidFbo(FboIndex* result) { return 0;};
void unlockAllFbo() {};
void setActiveFbo(FboIndex* fbo) {};
void activateDefaultDrawingProgram() {};
void activatePortalDrawingProgram() {};
void setClippingEquations(std::vector<glm::vec3>) {};

void TrianglesBufferInfo::draw() {}

TrianglesBufferInfo::TrianglesBufferInfo(
        std::span<glm::vec3> vertices,
        std::span<unsigned short> indices) : uvbuffer(0) {}

TrianglesBufferInfo::TrianglesBufferInfo(
        std::span<glm::vec3> vertices,
        std::span<glm::vec2> uv,
        std::span<unsigned short> indices) {}

TrianglesBufferInfo::~TrianglesBufferInfo() {}

void glewGenerateMipmap(uint32_t target){};
PFNGLGENERATEMIPMAPPROC __glewGenerateMipmap = glewGenerateMipmap;

struct ConsoleInit {
    ConsoleInit()
    {
        set_level(level::debug);
    }
};

ConsoleInit consoleInit;


void setPBR(float metallicFactor, float roughnessFactor)
{
    currentMetallicFactor = metallicFactor;
    currentRoughnessFactor = roughnessFactor;
}

void setTextureMode(unsigned int texture)
{
    isCurrentModeTexture = true;
    currentTexture = texture;
}

void setColoredMode(float color[3])
{
    isCurrentModeTexture = false;
    currentColor[0] = color[0];
    currentColor[1] = color[1];
    currentColor[2] = color[2];
}
