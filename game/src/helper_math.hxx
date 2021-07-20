#pragma once

#include <glm/mat4x4.hpp>
#include <string>

/**
 * @brief Helper function to print a matrix
 */
std::string mat4x4_to_string(glm::mat4x4);

/**
 * @brief Helper function to print a vec3
 */
std::string vec3_to_string(glm::vec3);

/**
 * @brief Helper function to print a vec4
 */
std::string vec4_to_string(glm::vec4);

/** @brief Get the equation of a plane given 1 point and a normal. Normal is normalized */
glm::vec4 getPlaneEquation(glm::vec3 p, glm::vec3 n);

/** @brief projection of a point on a plane.
 * Positive if in the same half-space as normal
 * Abs value is distance
 */
float pointPlaneProjection(glm::vec4 plane, glm::vec3 point);
