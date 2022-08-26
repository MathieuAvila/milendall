#pragma once

#include <glm/mat4x4.hpp>

float mat4x4_abs_diff(const glm::mat4x4& mat1, const glm::mat4x4& mat2);
bool check_equal_vec3(const glm::vec3 v1, const glm::vec3 v2);
extern float currentMetallicFactor;
extern float currentRoughnessFactor;
extern unsigned int currentTexture;
extern float currentColor[3];
extern bool isCurrentModeTexture;