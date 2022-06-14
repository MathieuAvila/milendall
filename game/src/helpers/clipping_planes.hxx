#pragma once

#include <glm/mat4x4.hpp>
#include <string>
#include <functional>

#include <gtest/gtest_prod.h>

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
        std::vector<glm::vec3> getEquations();
        /** @brief Is there something to print ? */
        bool isVoid() const;
        /** @brief for debug purposes */
        std::string toString();

    FRIEND_TEST(HelperClippingPlanes, ClippingPlanes_all_fit);
    FRIEND_TEST(HelperClippingPlanes, ClippingPlanes_none_fit);
    FRIEND_TEST(HelperClippingPlanes, ClippingPlanes_half_fit);
    FRIEND_TEST(HelperClippingPlanes, ClippingPlanes_half_fit_rot0);
    FRIEND_TEST(HelperClippingPlanes, ClippingPlanes_half_fit_rot1);
    FRIEND_TEST(HelperClippingPlanes, ClippingPlanes_half_fit_rot2);
    FRIEND_TEST(HelperClippingPlanes, ClippingPlanes_plane_purge_1_point);
    FRIEND_TEST(HelperClippingPlanes, ClippingPlanes_plane_purge_2_point);
};

std::string clippingEquationsToString(std::vector<glm::vec3> equ);