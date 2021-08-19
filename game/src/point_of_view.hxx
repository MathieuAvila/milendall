#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

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

    /** This is the local reference of the object. This is changed when crossing a portal in order
     * to let direction/up the same, while changing the local frame of reference for the PoV. */
    glm::mat3x3 local_reference;

    /** @brief current room this definition applies to. */
    std::string room;

    PointOfView() = default;
    PointOfView(const PointOfView& obj);
    PointOfView(const glm::vec3 _position, const glm::vec3 direction, const glm::vec3 up, glm::mat3x3 local_reference, const std::string _room);

    /** @brief Build a PoV from a position, angles and a local frame.
     * Angles are used to build direction and up vectors.
     */
    PointOfView(const glm::vec3 _position, const float vertical, const float horizontal, const glm::mat3x3 _local_reference, const std::string _room);

    bool operator==(const PointOfView&) const;

    PointOfView changeCoordinateSystem(std::string newRoom, const glm::mat4& newMatrix) const;
};

std::string to_string(const PointOfView&);