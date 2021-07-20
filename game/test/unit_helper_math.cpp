#include <gtest/gtest.h>

#include "common.hxx"
#include "helper_math.hxx"

static auto console = spdlog::stdout_color_mt("unit_helper_math");

glm::vec3 pO(0,0,0);
glm::vec3 pX(1,0,0);
glm::vec3 pY(0,1,0);
glm::vec3 pZ(0,0,1);

glm::vec3 vX(1,0,0);
glm::vec3 vY(0,1,0);
glm::vec3 vZ(0,0,1);

glm::vec3 v3A(1.0, 2.0, 3.0);
glm::vec4 v4A(1.0, 2.0, 3.0, 4.0);

TEST(HelperMath, getPlaneEquation) {
    auto equation_0_Z = getPlaneEquation(pO, vZ);
    ASSERT_EQ(equation_0_Z , glm::vec4(0,0,1,0) );

    auto equation_X_Z = getPlaneEquation(pX, vZ);
    ASSERT_EQ(equation_X_Z , glm::vec4(0,0,1,0) );

    auto equation_Z_Z = getPlaneEquation(pZ, vZ);
    ASSERT_EQ(equation_Z_Z , glm::vec4(0,0,1,-1) );

    auto equation_ZX_Z = getPlaneEquation(pZ + pX, vZ);
    ASSERT_EQ(equation_ZX_Z , glm::vec4(0,0,1,-1) );
}

TEST(HelperMath, getPlaneEquationNormalize) {

    auto equation_Z_Z = getPlaneEquation(pZ, vZ*2.0f);
    ASSERT_EQ(equation_Z_Z , glm::vec4(0,0,1,-1) );
}

TEST(HelperMath, pointPlaneProjection) {
    auto equation_0_Z = getPlaneEquation(pO, vZ);
    ASSERT_EQ(pointPlaneProjection(equation_0_Z , pO), 0.0);
    ASSERT_EQ(pointPlaneProjection(equation_0_Z , pX), 0.0);
    ASSERT_EQ(pointPlaneProjection(equation_0_Z , pZ), 1.0);
    ASSERT_EQ(pointPlaneProjection(equation_0_Z , -pZ), -1.0);

    auto equation_Z_Z = getPlaneEquation(pZ + pX, vZ);
    ASSERT_EQ(pointPlaneProjection(equation_Z_Z , -pZ), -2.0);
    ASSERT_EQ(pointPlaneProjection(equation_Z_Z , pX + pZ), 0.0);
    ASSERT_EQ(pointPlaneProjection(equation_Z_Z , pX), -1.0);
}

TEST(HelperMath, mat4x4_to_string) {

    ASSERT_EQ(mat4x4_to_string(glm::mat4(
    11.0 , 12.0, 13.0, 14.0,
    21.0 , 22.0, 23.0, 24.0,
    31.0 , 32.0, 33.0, 34.0,
    41.0 , 42.0, 43.0, 44.0)),
    "[\n[11.000000, 12.000000, 13.000000, 14.000000]\n"
    "[21.000000, 22.000000, 23.000000, 24.000000]\n"
    "[31.000000, 32.000000, 33.000000, 34.000000]\n"
    "[41.000000, 42.000000, 43.000000, 44.000000]\n]");
}

TEST(HelperMath, vec3_to_string) {

    ASSERT_EQ(vec3_to_string(v3A), "[ 1.000000 , 2.000000 , 3.000000 ]");
}

TEST(HelperMath, vec4_to_string) {

    ASSERT_EQ(vec4_to_string(v4A), "[ 1.000000 , 2.000000 , 3.000000 , 4.000000 ]");

}