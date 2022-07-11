#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "span.hxx"

#include "file_library.hxx"

int milendall_gl_init(FileLibrary& library);
void milendall_gl_close();

/** @brief Any exception related to loader */
class GlException: public std::runtime_error
{
    public:

        GlException(const std::string err): runtime_error(err) {};
};

/** @brief Reference to a valid FBO */
struct FboIndex {
    /** @brief the texture associated to it, to be used when available */
    unsigned int textureIndex;
    /** @brief the FBO intdex itself */
    unsigned int fboIndex;
    /** @brief RBO object */
    unsigned int rbo;
};

/** @brief Get a FBO from the FBO heap
 * @result 0 if OK, -1 if none available anymore
 */
int getValidFbo(struct FboIndex* result);

/** Signal to unlock all FBO, can be used again */
void unlockAllFbo();

/** Set active FBO */
void setActiveFbo(FboIndex* fbo);

/** Activate main program for drawing */
void activateDefaultDrawingProgram();

/** When default drawing program is used, commute between texture and
 * color with those 2 functions */
void setPBR(float metallicFactor, float roughnessFactor);
void setTextureMode(unsigned int texture);
void setColoredMode(float color[3]);

/** Activate portal program for drawing */
void activatePortalDrawingProgram();

/** Activate font program for drawing , returns program ID for use */
unsigned int activateFontDrawingProgram();

void updatePlayerInputs();

extern GLFWwindow* window;

extern float horizontalAngle;
extern float verticalAngle;

void setMeshMatrix(glm::mat4);
void setViewMatrix(glm::mat4x4 mat, glm::vec3 position);

void setClippingEquations(std::vector<glm::vec3>);

class TrianglesBufferInfo
{
    unsigned int vertexBuffer;
    unsigned int uvbuffer;
    unsigned int elementbuffer;
    unsigned int indicesCount;

    public:

    void draw();

    TrianglesBufferInfo(
        std::span<glm::vec3> vertices,
        std::span<glm::vec2> uv,
        std::span<unsigned short> indices);
    TrianglesBufferInfo(
        std::span<glm::vec3> vertices,
        std::span<unsigned short> indices);
    ~TrianglesBufferInfo();
};