#pragma once

#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>

#include "file_library.hxx"

int milendall_gl_init(FileLibrary& library);
void milendall_gl_close();

/** @brief Reference to a valid FBO */
struct FboIndex {
    /** @brief the texture associated to it, to be used when available */
    GLuint textureIndex;
    /** @brief the FBO intdex itself */
    GLuint fboIndex;
};

/** @brief Get a FBO from the FBO heap
 * @result 0 if OK, -1 if none available anymore
 */
int getValidFbo(FboIndex* result);

/** Release all FBO, can be used again */
int releaseAllFbo(FboIndex* result);

void computeMatricesFromInputs();

extern GLuint programID;
extern GLuint MatrixID;
extern GLuint TextureID;
extern GLFWwindow* window;
extern glm::mat4 ViewMatrix;
extern glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

extern glm::vec3 position;
extern glm::vec3 direction;
extern glm::vec3 up;

void setMeshMatrix(glm::mat4);
