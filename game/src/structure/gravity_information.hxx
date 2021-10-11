#pragma once

#include "glm/vec3.hpp"

#include "file_library.hxx"
#include "point_of_view.hxx"

/** @brief Result of gravity computation */
struct GravityInformation {
    /** @brief direction and force of gravity */
    glm::vec3 gravity;
    /** @brief direction in which the object points its head. This can be the same
     * as gravity (most cases), or something else */
    glm::vec3 up;
    /** @brief Duration of validity. 0 means it must always be recomputed, -1 means forever
     * (no recomputation needed) */
    float validity;
    /** @brief space kind */
     enum { GROUND, FLY, SWIM } space_kind;
};
