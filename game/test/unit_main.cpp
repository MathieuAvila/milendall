#include "common.hxx"

#include <GL/glew.h>

#include <stdint.h>

#include "gl_init.hxx"

void setMeshMatrix(glm::mat4 mat) {};
void setViewComponents(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {};
int getValidFbo(FboIndex* result) { return 0;};
void unlockAllFbo() {};
void setActiveFbo(FboIndex* fbo) {};
void activateDefaultDrawingProgram() {};
void activatePortalDrawingProgram() {};

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