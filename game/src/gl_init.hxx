#pragma once

#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>

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
int getValidFbo(FboIndex* result);

/** Signal to unlock all FBO, can be used again */
void unlockAllFbo();

/** Set active FBO */
void setActiveFbo(FboIndex* fbo);

/** Activate main program for drawing */
void activateDefaultDrawingProgram();

/** Activate portal program for drawing */
void activatePortalDrawingProgram();


void computeMatricesFromInputs();

extern GLFWwindow* window;
extern glm::mat4 ViewMatrix;
extern glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

extern glm::vec3 position;
extern glm::vec3 direction;
extern glm::vec3 up;

void setMeshMatrix(glm::mat4);
void setViewComponents(glm::vec3 position, glm::vec3 direction, glm::vec3 up);