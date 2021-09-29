#include <gtest/gtest.h>

#include "common.hxx"
#include "helper_math.hxx"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include "test_common.hpp"
#include <glm/gtc/matrix_transform.hpp>

static auto console = getConsole("unit_helper_math");

glm::vec3 pO(0,0,0);
glm::vec3 pX(1,0,0);
glm::vec3 pY(0,1,0);
glm::vec3 pZ(0,0,1);

glm::vec3 vX(1,0,0);
glm::vec3 vY(0,1,0);
glm::vec3 vZ(0,0,1);

glm::vec3 v3A(1.0, 2.0, 3.0);
glm::vec4 v4A(1.0, 2.0, 3.0, 4.0);

class HelperMathTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_helper_math")->set_level(spdlog::level::debug);
     spdlog::get("math")->set_level(spdlog::level::debug);
  }
};

TEST_F(HelperMathTest, getPlaneEquation) {
    auto equation_0_Z = getPlaneEquation(pO, vZ);
    ASSERT_EQ(equation_0_Z , glm::vec4(0,0,1,0) );

    auto equation_X_Z = getPlaneEquation(pX, vZ);
    ASSERT_EQ(equation_X_Z , glm::vec4(0,0,1,0) );

    auto equation_Z_Z = getPlaneEquation(pZ, vZ);
    ASSERT_EQ(equation_Z_Z , glm::vec4(0,0,1,-1) );

    auto equation_ZX_Z = getPlaneEquation(pZ + pX, vZ);
    ASSERT_EQ(equation_ZX_Z , glm::vec4(0,0,1,-1) );
}

TEST_F(HelperMathTest, getPlaneEquationNormalize) {

    auto equation_Z_Z = getPlaneEquation(pZ, vZ*2.0f);
    ASSERT_EQ(equation_Z_Z , glm::vec4(0,0,1,-1) );
}

TEST_F(HelperMathTest, pointPlaneProjection) {
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

TEST_F(HelperMathTest, mat4x4_to_string) {

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

TEST_F(HelperMathTest, vec3_to_string) {

    ASSERT_EQ(vec3_to_string(v3A), "[ 1.000000 , 2.000000 , 3.000000 ]");
}

TEST_F(HelperMathTest, vec4_to_string) {

    ASSERT_EQ(vec4_to_string(v4A), "[ 1.000000 , 2.000000 , 3.000000 , 4.000000 ]");

}

TEST_F(HelperMathTest, intersectSphereTrajectoryPlane_no_radius) {

    // plane is X/Y, starting at pZ
    // trajectory is from 0 to 2*pZ
    // sphere is a point , radius = 0.0
    // intersect at pZ.
    glm::vec3 position1(pO);
    glm::vec3 position2(pZ * 2.0f);
    glm::vec4 plane = getPlaneEquation(pZ, -vZ);
    glm::vec3 intersect_point;
    float distance;

    bool result = intersectSphereTrajectoryPlane(
        position1, position2, 0.0,
        plane,
        intersect_point,
        distance
    );
    console->info("result {}, intersect_point {}", result, vec3_to_string(intersect_point));
    ASSERT_TRUE(result);
    ASSERT_EQ(intersect_point, glm::vec3(0,0,1));
    ASSERT_EQ(distance, 1);
}

TEST_F(HelperMathTest, intersectSphereTrajectoryPlane_radius) {

    // plane is X/Y, starting at pZ
    // trajectory is from 0 to 2*pZ
    // sphere radius = 0.5
    // intersect at pZ.
    glm::vec3 position1(pO);
    glm::vec3 position2(pZ * 2.0f);
    glm::vec4 plane = getPlaneEquation(pZ, -vZ);
    glm::vec3 intersect_point;
    float distance;
    bool result = intersectSphereTrajectoryPlane(
        position1, position2, 0.5,
        plane,
        intersect_point,
        distance
    );
    console->info("result {}, intersect_point {}", result, vec3_to_string(intersect_point));
    ASSERT_TRUE(result);
    ASSERT_EQ(intersect_point, glm::vec3(0,0,0.5));
    ASSERT_EQ(distance, 0.5);
}

TEST_F(HelperMathTest, intersectSphereTrajectoryPlane_radius_inside_sphere) {

    // plane is X/Y, starting at pZ
    // trajectory is from 0 to 5*pZ
    // sphere radius = 4.0
    // intersect DIRECTLY at start of trajectory.
    glm::vec3 position1(pO);
    glm::vec3 position2(pZ * 5.0f);
    glm::vec4 plane = getPlaneEquation(pZ, -vZ);
    glm::vec3 intersect_point;
    float distance;
    bool result = intersectSphereTrajectoryPlane(
        position1, position2, 4.0,
        plane,
        intersect_point,
        distance
    );
    console->info("result {}, intersect_point {}", result, vec3_to_string(intersect_point));
    ASSERT_TRUE(result);
    ASSERT_EQ(intersect_point, position1);
    ASSERT_FLOAT_EQ(distance, 0.0f);
}

TEST_F(HelperMathTest, intersectSphereTrajectoryPlane_bad_direction) {

    // plane is X/Y, starting at pZ
    // trajectory is from 2*pZ to 0
    // sphere radius = 0.5
    // intersect at pZ, but will return FALSE because it's not the right orientation.
    glm::vec3 position1(pZ * 2.0f);
    glm::vec3 position2(pO);
    glm::vec4 plane = getPlaneEquation(pZ, -vZ);
    glm::vec3 intersect_point;
    float distance;
    bool result = intersectSphereTrajectoryPlane(
        position1, position2, 0.5,
        plane,
        intersect_point,
        distance
    );
    console->info("result {}, intersect_point {}", result, vec3_to_string(intersect_point));
    ASSERT_FALSE(result);
}

TEST_F(HelperMathTest, intersectSphereTrajectoryPlane_bad_direction_reversed) {

    // plane is X/Y, starting at pZ
    // trajectory is from 2*pZ to 0
    // sphere radius = 0.5
    // intersect at pZ, but will return TRUE because it's not the right orientation BUT with reversed request.
    glm::vec3 position1(pZ * 2.0f);
    glm::vec3 position2(pO);
    glm::vec4 plane = getPlaneEquation(pZ, -vZ);
    glm::vec3 intersect_point;
    float distance;
    bool result = intersectSphereTrajectoryPlane(
        position1, position2, 0.5,
        plane,
        intersect_point,
        distance,
        true
    );
    console->info("result {}, intersect_point {}", result, vec3_to_string(intersect_point));
    ASSERT_TRUE(result);
}


TEST_F(HelperMathTest, intersectSphereTrajectorySegment1_radius1) {
    // Simple check with sphere radius 1

    // Segment : A = - 2*pZ, B = + 2*pZ ==> follows Z around Origin
    // trajectory is from -2 pX to 2 px
    // sphere is a point , radius = 1.0
    glm::vec3 position1(pX * -2.0f);
    glm::vec3 position2(pX * 2.0f);
    glm::vec3 A = - pZ * 2.0f;
    glm::vec3 B = + pZ * 2.0f;

    glm::vec3 intersect_center, normal;
    float distance;
    bool result = intersectSphereTrajectorySegment(
        position1, position2, 1.0f,
        A, B,
        intersect_center, distance, normal);
    ASSERT_TRUE(result);
    ASSERT_EQ(intersect_center, glm::vec3(-1.0, 0.0, 0.0));
    ASSERT_EQ(distance, 1.0f);
    ASSERT_EQ(normal, glm::vec3(-1.0, 0.0, 0.0));

};

TEST_F(HelperMathTest, intersectSphereTrajectorySegment2_radius15) {

    // Simple check with sphere radius 1.5, 2 solutions, first solution

    // Segment : A = - 2*pZ, B = + 2*pZ ==> follows Z around Origin
    // trajectory is from -2 pX to 2 px
    // sphere is a point , radius = 1.5
    glm::vec3 position1(pX * -2.0f);
    glm::vec3 position2(pX * 2.0f);
    glm::vec3 A = - pZ * 2.0f;
    glm::vec3 B = + pZ * 2.0f;

    glm::vec3 intersect_center, normal;
    float distance;
    bool result = intersectSphereTrajectorySegment(
        position1, position2, 1.5f,
        A, B,
        intersect_center, distance, normal);
    ASSERT_TRUE(result);
    ASSERT_EQ(intersect_center, glm::vec3(-1.5, 0.0, 0.0));
    ASSERT_EQ(distance, 0.5f);
    ASSERT_EQ(normal, glm::vec3(-1.0, 0.0, 0.0));
};

TEST_F(HelperMathTest, intersectSphereTrajectorySegment3_radius15_at_origin) {

    // Simple check with sphere radius 1.5, 2 solutions with origin between the 2,
    // so that it is already in it.

    // Segment : A = - 1, B = + 2*pZ ==> follows Z around Origin
    // trajectory is from -0.5 pX to 2 px
    // sphere is a point , radius = 1.5
    glm::vec3 position1(pX * -0.5f); // past 1st solution
    glm::vec3 position2(pX * 2.0f);
    glm::vec3 A = - pZ * 2.0f;
    glm::vec3 B = + pZ * 2.0f;

    glm::vec3 intersect_center, normal;
    float distance;
    bool result = intersectSphereTrajectorySegment(
        position1, position2, 1.5f,
        A, B,
        intersect_center, distance, normal);
    ASSERT_TRUE(result);
    ASSERT_EQ(intersect_center, position1);
    ASSERT_EQ(distance, 0.0f); // start point
    ASSERT_EQ(normal, glm::vec3(-1.0, 0.0, 0.0));
};


TEST_F(HelperMathTest, intersectSphereTrajectorySegment4_radius15_not_center) {

    // Segment : A = - 2*pZ + pY, B = + 2*pZ + pY==> follows Z around Origin
    // trajectory is from -2 pX to 2 px
    // sphere is a point , radius = 1.5
    glm::vec3 position1(pX * -2.0f + pY);
    glm::vec3 position2(pX * 2.0f + pY);
    glm::vec3 A = - pZ * 2.0f;
    glm::vec3 B = + pZ * 2.0f;

    glm::vec3 intersect_center, normal;
    float distance;
    bool result = intersectSphereTrajectorySegment(
        position1, position2, 1.5f,
        A, B,
        intersect_center, distance, normal);
    ASSERT_TRUE(result);
    ASSERT_EQ(distance, 0.881966f);
    ASSERT_TRUE(0.01 > glm::length(normal - glm::vec3(-0.745356 , 0.666667 , 0.0))); // XXX should be computed
};


TEST_F(HelperMathTest, getRotatedMatrix_0_ID) {

    glm::mat3 ID = getRotatedMatrix(0.0f, 0.0f);
    ASSERT_EQ(ID, glm::mat3(1.0f));
}

TEST_F(HelperMathTest, getRotatedMatrix_1_rotate_horiz) {

    glm::mat3 newMat = getRotatedMatrix(0.0f, glm::pi<float>() / 4.0);
    glm::mat3 matCheck{
        0.7, 0.0, -0.7,
        0.0, 1.0, 0.0,
        0.7, 0.0, 0.7
    }; // rotation PI/4 on Y

    auto diff = mat4x4_abs_diff(matCheck, newMat);
    console->info("newMat {}", mat4x4_to_string(newMat));
    ASSERT_TRUE(glm::epsilonEqual(diff, 0.0f, 0.1f));
}

TEST_F(HelperMathTest, getRotatedMatrix_2_rotate_vertical) {

    glm::mat3 newMat = getRotatedMatrix(glm::pi<float>() / 4.0, 0.0f);
    glm::mat3 matCheck{
        1.0, 0.0, 0.0,
        0.0, 0.7, 0.7,
        0.0, -0.7, 0.7
    }; // rotation PI/4 on X

    auto diff = mat4x4_abs_diff(matCheck, newMat);
    console->info("newMat {}", mat4x4_to_string(newMat));
    ASSERT_TRUE(glm::epsilonEqual(diff, 0.0f, 0.1f));
}

TEST_F(HelperMathTest, getRotatedMatrix_3_rotate_vertical_horizontal) {

    glm::mat3 newMat = getRotatedMatrix(glm::pi<float>() / 4.0, glm::pi<float>() / 4.0);
    glm::mat3 matCheck{
        0.7, 0.0, -0.7,
        0.5, 0.7, 0.5,
        0.5, -0.7, 0.5
    }; // rotation PI/4 on X, rotation PI/4 on Y

    auto diff = mat4x4_abs_diff(matCheck, newMat);
    console->info("newMat {}", mat4x4_to_string(newMat));
    ASSERT_TRUE(glm::epsilonEqual(diff, 0.0f, 0.1f));
}

TEST_F(HelperMathTest, moveOnPlane) {

    glm::vec3 start(10.0f,1.0f,0.0f);
    glm::vec3 end(10.0f,0.0f,1.0f);
    glm::vec3 normal(0.0,1.0f,0.0f);
    float coeff = 0.1f;
    glm::vec3 result = moveOnPlane(start, end, normal, coeff);
    console->info("result {}", vec3_to_string(result));
    ASSERT_TRUE(result == glm::vec3(10.0f, 1.2f, 1.0f));
}

TEST_F(HelperMathTest, checkAdherenceCone) {
    ASSERT_TRUE(checkAdherenceCone(glm::vec3(0.0f, 1.0f ,0.0f), glm::vec3(0.0f, -1.0f ,0.0f), 0.5));
    ASSERT_FALSE(checkAdherenceCone(glm::vec3(0.0f, 1.0f ,0.0f), glm::vec3(0.0f, 1.0f ,0.0f), 0.5));

    ASSERT_TRUE(checkAdherenceCone(glm::vec3(0.0f, 1.0f ,0.0f), glm::vec3(0.0f, -5.0f , 5.0f), 0.5)); // check noramlize
    ASSERT_FALSE(checkAdherenceCone(glm::vec3(0.0f, 1.0f ,0.0f), glm::vec3(0.0f, 5.0f , 5.0f), 0.5)); // check noramlize
}

TEST_F(HelperMathTest, computeRotatedMatrix_0_id_no_rotate) {

    glm::vec3 z0(0.0f, -1.0f, 0.0f);
    glm::mat3x3 result = computeRotatedMatrix(
        glm::mat3x3(1.0f),
        z0,
        [](float originalAngle)
        {
            return 0.0f;
        } );
    console->debug("final {}", mat4x4_to_string(result));
    ASSERT_EQ(result, glm::mat3x3(1.0f));
}

TEST_F(HelperMathTest, computeRotatedMatrix_1_NOT_id_no_rotate) {

    glm::vec3 z0(0.0f, -1.0f, 0.0f);
    glm::mat3x3 result = computeRotatedMatrix(
        glm::mat3x3(2.0f),
        z0,
        [](float originalAngle)
        {
            return 0.0f;
        } );
    console->debug("final {}", mat4x4_to_string(result));
    ASSERT_EQ(result, glm::mat3x3(2.0f));
}

TEST_F(HelperMathTest, computeRotatedMatrix_2_id_transform_Y_to_X) {

    glm::mat3x3 original = glm::mat4x4(1.0f);
    glm::mat3x3 result = computeRotatedMatrix(
        original,
        vX,
        [](float originalAngle)
        {
            return originalAngle;
        } );
    console->debug("original {}", mat3x3_to_string(original));
    console->debug("final {}", mat3x3_to_string(result));
    console->debug("new_up {}", vec3_to_string(result * vY));
    ASSERT_TRUE(glm::bvec3(true,true,true) == glm::epsilonEqual(result * vY, vX, 0.1f));
}

TEST_F(HelperMathTest, computeRotatedMatrix_3_NOT_id_transform_Y_to_X) {

    glm::mat3x3 original = glm::rotate(
        glm::rotate(glm::mat4x4(1.0f), glm::pi<float>()/4.0f, vZ)
        , glm::pi<float>()/4.0f, vX); // rotate X and Y and Z
    glm::mat3x3 result = computeRotatedMatrix(
        original,
        vX,
        [](float originalAngle)
        {
            return originalAngle;
        } );
    console->debug("original {}", mat3x3_to_string(original));
    console->debug("final {}", mat3x3_to_string(result));
    console->debug("new_up {}", vec3_to_string(result * vY));
    ASSERT_TRUE(glm::bvec3(true,true,true) == glm::epsilonEqual(result * vY, vX, 0.1f));
}

TEST_F(HelperMathTest, computeRotatedMatrix_3_NOT_id_rotate_x_to_pi2) {

    glm::mat3x3 original = glm::rotate(glm::mat4x4(1.0f), glm::pi<float>()/4.0f, vX);
    glm::mat3x3 result = computeRotatedMatrix(
        original,
        vZ,
        [](float originalAngle)
        {
            return glm::pi<float>()/4.0f;
        } );
    glm::mat3x3 expected = glm::rotate(glm::mat4x4(1.0f), glm::pi<float>()/2.0f, vX);

    console->debug("original {}", mat3x3_to_string(original));
    console->debug("final {}", mat3x3_to_string(result));
    console->debug("expected {}", mat3x3_to_string(expected));
    console->debug("diff {}", mat3x3_to_string(result - expected));

    ASSERT_TRUE(mat4x4_abs_diff(result, expected) <  0.01f );
}

TEST_F(HelperMathTest, computeRotatedMatrix_4_rotY_pi4___UP_aligned) {

    glm::mat3x3 original = glm::rotate(glm::mat4x4(1.0f), glm::pi<float>()/4.0f, vY);
    glm::mat3x3 result = computeRotatedMatrix(
        original,
        vY,
        [](float originalAngle)
        {
            return -originalAngle;
        } );
    glm::mat3x3 expected = original;

    console->debug("original {}", mat3x3_to_string(original));
    console->debug("final {}", mat3x3_to_string(result));

    ASSERT_TRUE(mat4x4_abs_diff(result, expected) <  0.01f );
}

TEST_F(HelperMathTest, computeRotatedMatrix_5_rotZ_pi4___going_Y) {

    glm::mat3x3 original = glm::rotate(glm::mat4x4(1.0f), glm::pi<float>()/2.0f, vZ);
    glm::mat3x3 result = computeRotatedMatrix(
        original,
        vY,
        [](float originalAngle)
        {
            return originalAngle;
        } );
    glm::mat3x3 expected = glm::mat4x4(1.0f);

    console->debug("original {}", mat3x3_to_string(original));
    console->debug("final {}", mat3x3_to_string(result));

    ASSERT_TRUE(mat4x4_abs_diff(result, expected) <  0.01f );
}
