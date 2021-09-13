#include <gtest/gtest.h>

#include "common.hxx"
#include "helper_math.hxx"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>

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
    "[\n[11.0, 12.0, 13.0, 14.0]\n"
    "[21.0, 22.0, 23.0, 24.0]\n"
    "[31.0, 32.0, 33.0, 34.0]\n"
    "[41.0, 42.0, 43.0, 44.0]\n]");
}

TEST(HelperMath, vec3_to_string) {

    ASSERT_EQ(vec3_to_string(v3A), "[ 1.0 , 2.0 , 3.0 ]");
}

TEST(HelperMath, vec4_to_string) {

    ASSERT_EQ(vec4_to_string(v4A), "[ 1.0 , 2.0 , 3.0 , 4.0 ]");

}

TEST(HelperMath, intersectSphereTrajectoryPlane_no_radius) {

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

TEST(HelperMath, intersectSphereTrajectoryPlane_radius) {

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

TEST(HelperMath, intersectSphereTrajectoryPlane_bad_direction) {

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

TEST(HelperMath, intersectSphereTrajectoryPlane_bad_direction_reversed) {

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


TEST(HelperMath, intersectSphereTrajectorySegment1_radius1) {
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

TEST(HelperMath, intersectSphereTrajectorySegment2_radius15) {

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

TEST(HelperMath, intersectSphereTrajectorySegment3_radius15_at_origin) {

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


TEST(HelperMath, intersectSphereTrajectorySegment4_radius15_not_center) {

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


TEST(HelperMath, getRotatedMatrix_0_ID) {

    glm::mat3 ID = getRotatedMatrix(0.0f, 0.0f);
    ASSERT_EQ(ID, glm::mat3(1.0f));
}

float mat4x4_abs_diff(const glm::mat4x4& mat1, const glm::mat4x4& mat2)
{
	float result = 0.0f;
	for(glm::length_t i = 0; i < 4; ++i)
	    for(glm::length_t j = 0; j < 4; ++j)
		result += abs(mat1[i][j] - mat2[i][j]);
	return result;
}


TEST(HelperMath, getRotatedMatrix_1_rotate_horiz) {

    glm::mat3 newMat = getRotatedMatrix(0.0f, glm::pi<float>() / 4.0);
    glm::mat3 matCheck{
        0.7, 0.0, -0.7,
        0.0, 1.0, 0.0,
        0.7, 0.0, 0.7
    }; // rotation PI/4 on Y

    auto diff = mat4x4_abs_diff(matCheck, newMat);
    console->info("newMat {}", mat4x4_to_string(newMat));
    ASSERT_TRUE(glm::epsilonEqual(diff, 0.0f, 0.2f));
}

TEST(HelperMath, getRotatedMatrix_2_rotate_vertical) {

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

TEST(HelperMath, getRotatedMatrix_3_rotate_vertical_horizontal) {

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
