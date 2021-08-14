#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <string>

/** Position and direction of an object inside a room.
 */
struct PointOfView
{
    /** @brief current position, will be updated */
    glm::vec3 position;

    /** @brief direction vector */
    glm::vec3 direction;

    /** @brief up vector */
    glm::vec3 up;

    /** @brief current room this definition applies to. */
    std::string room;

    PointOfView() = default;
    PointOfView(PointOfView& obj) = default;

    PointOfView changeCoordinateSystem(std::string newRoom, glm::mat4& newMatrix);
};
