#pragma once

#include <nlohmann/json.hpp>
#include <glm/mat4x4.hpp>

/**
 * @brief Helper function to get a matrix from a gltf file
 */
glm::mat4x4 jsonGetMatrix4x4(nlohmann::json&);

/**
 * @brief Helper function to get a vec3 from a gltf file
 */
glm::vec3 jsonGetVec3(nlohmann::json&);

/**
 * @brief Helper function to get a vec4 from a gltf file
 */
glm::vec4 jsonGetVec4(nlohmann::json&);
