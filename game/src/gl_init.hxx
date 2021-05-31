#pragma once

#include <GLFW/glfw3.h>

int milendall_gl_init();
void milendall_gl_close();

void computeMatricesFromInputs();

extern GLuint programID;
extern GLuint MatrixID;
extern GLuint TextureID;
extern GLFWwindow* window;
extern glm::mat4 ViewMatrix;
extern glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();