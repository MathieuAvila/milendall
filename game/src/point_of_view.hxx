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

    /** @brief get "forward" vector, computed from local_reference */
    glm::vec3 getDirection() const;

    /** @brief get "up" vector, computed from local_reference */
    glm::vec3 getUp() const;

    /** @brief get "right" vector, computed from local_reference */
    glm::vec3 getRight() const;

    /** This is the local reference of the object. This is changed when crossing a portal in order
     * to let direction/up the same, while changing the local frame of reference for the PoV. */
    glm::mat3x3 local_reference;

    /** @brief current room this definition applies to. */
    std::string room;

    PointOfView() = default;
    PointOfView(const PointOfView& obj);
    PointOfView(const glm::vec3 _position, glm::mat3x3 local_reference, const std::string _room);

    bool operator==(const PointOfView&) const;

    /** @brief Build a new PoV when a change of room occurs */
    PointOfView changeCoordinateSystem(std::string newRoom, const glm::mat4& newMatrix) const;

    /** @brief Build a new PoV where matrix is premultipled by another frame of reference */
    PointOfView prependCoordinateSystem(const glm::mat3& newMatrix) const;

    /** @brief Return ViewMatrix as expected by GL */
    glm::mat4x4 getViewMatrix() const;
};

std::string to_string(const PointOfView&);