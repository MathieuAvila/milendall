#include <gtest/gtest.h>

#include "common.hxx"
#include "clipping_planes.hxx"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include "test_common.hpp"
#include <glm/gtc/matrix_transform.hpp>

static auto console = getConsole("unit_clipping_planes");

class HelperClippingPlanes : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_clipping_planes")->set_level(spdlog::level::debug);
     spdlog::get("clipping_planes")->set_level(spdlog::level::debug);
  }
};

TEST_F(HelperClippingPlanes, ClippingPlanes_all_fit) {

    ClippingPlanes clipping, clippingOrg;

    // plane x = -10
    clipping.clipByPlane(glm::vec3(1.0, 0.0, 10.0));

    console->debug("equ ref:\n{}", clippingOrg.toString());
    console->debug("equ res:\n{}", clipping.toString());

    ASSERT_EQ(clipping.getEquations(), clippingOrg.getEquations() );
}

TEST_F(HelperClippingPlanes, ClippingPlanes_none_fit) {

    ClippingPlanes clipping;

    // plane x = 10
    clipping.clipByPlane(glm::vec3(1.0, 0.0, -10.0));

    console->debug("equ res:\n{}", clipping.toString());

    ASSERT_EQ(clipping.getEquations(), std::vector<glm::vec3>());
}

TEST_F(HelperClippingPlanes, ClippingPlanes_half_fit) {
    ClippingPlanes clipping;
    // plane x = 0
    clipping.clipByPlane(glm::vec3(1.0, 0.0, 0.0));
    console->debug("equ res:\n{}", clipping.toString());
    ASSERT_EQ(clipping.currentPoints,
        std::vector<glm::vec2>( {
            glm::vec2(1.01,1.01),
            glm::vec2(1.01,-1.01),
            glm::vec2(0,-1.01),
            glm::vec2(0,1.01)
        }));
}

TEST_F(HelperClippingPlanes, ClippingPlanes_half_fit_rot0) {
    ClippingPlanes clipping;
    // plane y = 0
    clipping.clipByPlane(glm::vec3(0.0, 1.0, 0.0));
    console->debug("equ res:\n{}", clipping.toString());
    ASSERT_EQ(clipping.currentPoints,
        std::vector<glm::vec2>( {
            glm::vec2(-1.01,  1.01),
            glm::vec2(1.01,   1.01),
            glm::vec2(1.01,   0.0),
            glm::vec2(-1.01,  0.0)
        }));
}

TEST_F(HelperClippingPlanes, ClippingPlanes_half_fit_rot1) {
    ClippingPlanes clipping;
    // plane -x = 0
    clipping.clipByPlane(glm::vec3(-1.0, 0.0, 0.0));
    console->debug("equ res:\n{}", clipping.toString());
    ASSERT_EQ(clipping.currentPoints,
        std::vector<glm::vec2>( {
            glm::vec2(-1.01,  -1.01),
            glm::vec2(-1.01,  1.01),
            glm::vec2(0.0,    1.01),
            glm::vec2(0.0,    -1.01)
        }));
}

TEST_F(HelperClippingPlanes, ClippingPlanes_half_fit_rot2) {
    ClippingPlanes clipping;
    // plane x = 0
    clipping.clipByPlane(glm::vec3(0.0, -1.0, 0.0));
    console->debug("equ res:\n{}", clipping.toString());
    ASSERT_EQ(clipping.currentPoints,
        std::vector<glm::vec2>( {
            glm::vec2(1.01,   -1.01),
            glm::vec2(-1.01,  -1.01),
            glm::vec2(-1.01,   0.0),
            glm::vec2(1.01,    0.0)
        }));
}


 TEST_F(HelperClippingPlanes, ClippingPlanes_plane_purge_1_point)
 {
    ClippingPlanes clipping(1.0,1.0);
    // plane x+y = 2.0
    clipping.clipByPlane(glm::vec3(1.0, 1.0, -2.0));
    console->debug("equ res:\n{}", clipping.toString());
    ASSERT_EQ(clipping.currentPoints,
        std::vector<glm::vec2>({}));
 }

 TEST_F(HelperClippingPlanes, ClippingPlanes_plane_purge_2_point)
 {
    ClippingPlanes clipping(1.0,1.0);
    // plane x = 1.0
    clipping.clipByPlane(glm::vec3(1.0, 0.0, -1.0));
    console->debug("equ res:\n{}", clipping.toString());
    ASSERT_EQ(clipping.currentPoints,
        std::vector<glm::vec2>({}));
 }

 TEST_F(HelperClippingPlanes, ClippingPlanes_polygon_all_in)
 {
    ClippingPlanes clipping(1.0,1.0);
    std::vector<glm::vec3> poly(
        {
            glm::vec3(10.0, 10.0, 1.0),
            glm::vec3(-10.0, 10.0, 1.0),
            glm::vec3(-10.0, -10.0, 1.0),
            glm::vec3(10.0, -10.0, 1.0),
        });
    clipping.clipByPolygon(poly);
    console->debug("equ res:\n{}", clipping.toString());
    ASSERT_EQ(clipping.getEquations(),
        std::vector<glm::vec3>({}));
 }