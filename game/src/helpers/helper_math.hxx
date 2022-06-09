#pragma once

#include <glm/mat4x4.hpp>
#include <string>
#include <functional>

#include <gtest/gtest_prod.h>

/** @brief Helper function to print a matrix */
std::string mat4x4_to_string(glm::mat4x4);

/** @brief Helper function to print a matrix */
std::string mat3x3_to_string(glm::mat3x3);

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

/** @brief Useful to append the extra W=1 */
glm::vec4 positionToVec4(glm::vec3);

/** @brief Useful to append the extra W=0 */
glm::vec4 vectorToVec4(glm::vec3);

/** @brief Sphere trajectory intersection with plane.
 * @param radius sphere radius
 * @param position1 sphere start position
 * @param position2 sphere end position
 * @param plane plane equation
 * @param intersect_center OUT center position at point of intersection
 * @param distance OUT distance from position1
 * @returns TRUE if it intersects
*/
bool intersectSphereTrajectoryPlane(
    glm::vec3 position1, glm::vec3 position2, float radius,
    glm::vec4 plane,
    glm::vec3& intersect_center, float& distance,
    bool reversed_face = false
    );

/** @brief Sphere trajectory intersection with segment.
 * @param radius sphere radius
 * @param position1 sphere start position
 * @param position2 sphere end position
 * @param plane plane equation
 * @param intersect_center OUT center position at point of intersection
 * @param distance OUT distance from position1
 * @returns TRUE if it intersects
*/
bool intersectSphereTrajectorySegment(
    glm::vec3 position1, glm::vec3 position2, float radius,
    glm::vec3 A, glm::vec3 B,
    glm::vec3& intersect_center, float& distance, glm::vec3& normal
    );

/** @brief get a matrix with an horizontal and vertical rotation */
glm::mat3 getRotatedMatrix(float verticalAngle, float horizontalAngle);

/** @brief Project a movement on a normal plane, and apply a coefficient to move from it */
glm::vec3 moveOnPlane(glm::vec3 start, glm::vec3 end, glm::vec3 normal, float coeff);

/** @brief Compute whether a move on a wall reaches a given angle, or adherence. */
bool checkAdherenceCone(glm::vec3 normal, glm::vec3 graivty, float adherence);

/** @brief Produce a new matrix that includes a rotation to the old one, alongside
 *         an axis given by 1 vector and (0,1,0) (aka "up"), and an angle provided
 *         by a function passed in parameter
 *         This function is taking 1 parameter: the original angle between the
 *         2 vectors
 *         If the angle cannot be computed, a small deviation is added so that
 *         it becomes to compute one.
 * @param original the original matrix
 * @param second_vector The second vector for computation of the angle
 * @param angle_provider The function that returns the angle to apply based on the original one
 * */
glm::mat3x3 computeRotatedMatrix(glm::mat3x3 original, glm::vec3 second_vector, std::function<float(float)> angle_provider );

/**
 * @brief Cut a 2D segment defined by p0/p1 in two, by a plane defined by a 2D equation plane.
 * no use outside ClippingPlanes, exposed here for test purposes.
 */
glm::vec2 plane2DCutPoint(glm::vec3 plane, glm::vec2 p0, glm::vec2 p1);

/** @brief Clipping facility
 *         Builds a list of clipping equations and helps cut them.
 */
class ClippingPlanes
{
    std::vector<glm::vec2> currentPoints;

    /* internal clipping by a 2-D line set on Z=1.0 */
    void clipByPlane(glm::vec3 plane);

    public:
        /** @brief initialize to a standard cone view, for test purposes. Defaults to 1.01 / 1.01 */
        ClippingPlanes();
        /** @brief initialize to a cone view with specific aspect ratio */
        ClippingPlanes(float width, float height);
        /** @brief cut by a polygon intersect, as defined by a list of points */
        void clipByPolygon(std::vector<glm::vec3>&);
        /** @brief get a list of planes equations
         * defined by a list of X,Y values. Z=1.0, U=0.0
         * If void, everything is discarded*/
        std::vector<glm::vec2> getEquations();
        /** @brief for debug purposes */
        std::string toString();

    FRIEND_TEST(HelperMathTest, ClippingPlanes_all_fit);
    FRIEND_TEST(HelperMathTest, ClippingPlanes_none_fit);
    FRIEND_TEST(HelperMathTest, ClippingPlanes_half_fit);
    FRIEND_TEST(HelperMathTest, ClippingPlanes_half_fit_rot0);
    FRIEND_TEST(HelperMathTest, ClippingPlanes_half_fit_rot1);
    FRIEND_TEST(HelperMathTest, ClippingPlanes_half_fit_rot2);
    FRIEND_TEST(HelperMathTest, ClippingPlanes_plane_purge_1_point);
    FRIEND_TEST(HelperMathTest, ClippingPlanes_plane_purge_2_point);
};
