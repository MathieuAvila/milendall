#pragma once

#include "glm/vec3.hpp"

#include "file_library.hxx"
#include "point_of_view.hxx"
#include "helper_math.hxx"
#include <list>

/** @brief Result of gravity computation */
struct GravityInformation {

    typedef enum { GROUND, FLY, SWIM } SpaceKind;

    /** @brief direction and force of gravity */
    glm::vec3 gravity;
    /** @brief direction in which the object points its head. This can be the same
     * as gravity (most cases), or something else */
    glm::vec3 up;
    /** @brief Duration of validity. 0 means it must always be recomputed, -1 means forever
     * (no recomputation needed) */
    float validity;
    /** @brief space kind */
    SpaceKind space_kind;
    /** @brief Weight of this graivty information. This is only meaningful when
     * traversing different spaces with different gravities */
    float weight;

    /** default constructor will build reasonable values, with none validity */
    GravityInformation();

    /** constructor with all values*/
    GravityInformation(glm::vec3 _gravity, glm::vec3 _up, float _validity, SpaceKind _space_kind, float _weight);

    /** constructor with assumption of ground */
    GravityInformation(glm::vec3 _gravity, glm::vec3 _up, float _validity, float _weight);

    /** constructor from a combined list of gravity information. ATM, mix them */
    GravityInformation(std::list<GravityInformation> sources);

    /** operator== should be used for UT only  */
    bool operator==(const GravityInformation&) const;
};
