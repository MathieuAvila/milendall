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

/** Activate portal program for drawing */
void activatePortalDrawingProgram();

/** Activate font program for drawing , returns program ID for use */
unsigned int activateFontDrawingProgram();

void computeMatricesFromInputs();

extern GLFWwindow* window;
extern glm::mat4 ViewMatrix;
extern glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

extern glm::vec3 position;
extern glm::vec3 direction;
extern glm::vec3 up;

extern float horizontalAngle;
extern float verticalAngle;

void setMeshMatrix(glm::mat4);
void setViewComponents(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

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