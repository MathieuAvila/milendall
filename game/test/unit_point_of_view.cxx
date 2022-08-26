#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "common.hxx"
#include <point_of_view.hxx>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include "helper_math.hxx"
#include "test_common.hxx"

static auto console = getConsole("ut_pov");

TEST(PointOfView, changeCoordinateSystem)
{
    PointOfView pov{
        glm::vec3(1.0, 0.0, 0.0),
        glm::mat3x3(1.0f),
        "myRoom"
    };
    auto rotate = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0, 1.0, 0.0));
    rotate = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0, 0.0, 1.0));

    glm::mat4 newMatrix = glm::translate(rotate, glm::vec3(0.0f, 1.0f, 0.0f));
    auto updated = pov.changeCoordinateSystem("newRoom", newMatrix);

    console->info("{}", to_string(updated));
    console->info("up {}", vec3_to_string(updated.getUp()));
    console->info("direction {}", vec3_to_string(updated.getDirection()));

    EXPECT_TRUE(glm::distance(updated.position, glm::vec3(-1.0f, -1.0f, 0.0f)) < 0.01f);
    EXPECT_TRUE(glm::distance(updated.getUp(), glm::vec3(0.0f, -1.0f, 0.0f))< 0.01f);
    EXPECT_TRUE(glm::distance(updated.getDirection(), glm::vec3(0.0f, 0.0f, 1.0f))< 0.01f);
    EXPECT_EQ(updated.room, "newRoom");
}

TEST(PointOfView, initAngles)
{
    PointOfView pov{
        glm::vec3(1.0, 2.0, 3.0),
        glm::mat3x3(2.0f),
        "myRoom"
    };
    console->info("{}", to_string(pov));
    console->info("up {}", vec3_to_string(pov.getUp()));
    console->info("direction {}", vec3_to_string(pov.getDirection()));
    EXPECT_TRUE(glm::distance(pov.position, glm::vec3(1.0f, 2.0f, 3.0f)) < 0.05f);
    EXPECT_TRUE(glm::distance(pov.getUp(), glm::vec3( 0.0, 2.0, 0.0))< 0.05f);
    EXPECT_TRUE(glm::distance(pov.getDirection(), glm::vec3(0.0, 0.0 , 2.0))< 0.05f);
    EXPECT_EQ(glm::mat3x3(2.0f), pov.local_reference);
    EXPECT_EQ(pov.room, "myRoom");
}

TEST(PointOfView, prependCoordinateSystem_0_ID )
{
    PointOfView pov{
        glm::vec3(1.0, 2.0, 3.0),
        getRotatedMatrix(0.0f, glm::pi<float>() / 4.0),
        "myRoom"
    };

    auto pov2 = pov.prependCoordinateSystem(glm::mat4(1.0f));

    console->info("{}", to_string(pov2));

    glm::mat3 matCheck{
        0.7, 0.0, -0.7,
        0.0, 1.0, 0.0,
        0.7, 0.0, 0.7
    }; // rotation PI/4 on Y

    auto diff = mat4x4_abs_diff(matCheck, pov2.local_reference);
    ASSERT_TRUE(glm::epsilonEqual(diff, 0.0f, 0.1f));
}

TEST(PointOfView, prependCoordinateSystem_1_new_mat )
{
    PointOfView pov{
        glm::vec3(1.0, 2.0, 3.0),
        getRotatedMatrix(0.0f, glm::pi<float>() / 4.0),
        "myRoom"
    };
    auto pov2 = pov.prependCoordinateSystem(getRotatedMatrix(glm::pi<float>() / 4.0, 0.0f));
    console->info("{}", to_string(pov2));
    glm::mat3 matCheck{
        0.7, 0.0, -0.7,
        0.5, 0.7, 0.5,
        0.5, -0.7, 0.5
    };
    auto diff = mat4x4_abs_diff(matCheck, pov2.local_reference);
    ASSERT_TRUE(glm::epsilonEqual(diff, 0.0f, 0.1f));
}

TEST(PointOfView, getDirection )
{
    PointOfView pov{
        glm::vec3(1.0, 2.0, 3.0),
        getRotatedMatrix(0.0f, glm::pi<float>() / 4.0),
        "myRoom"
    };
    auto direction = pov.getDirection();
    console->info("{}", vec3_to_string(direction));
    ASSERT_TRUE(glm::distance(direction, glm::vec3(0.7, 0.0, 0.7))< 0.05f);
}

TEST(PointOfView, getUp )
{
    PointOfView pov{
        glm::vec3(1.0, 2.0, 3.0),
        getRotatedMatrix(glm::pi<float>() / 4.0, 0.0f),
        "myRoom"
    };
    auto up = pov.getUp();
    console->info("{}", vec3_to_string(up));
    ASSERT_TRUE(glm::distance(up, glm::vec3(0.0, 0.7, 0.7))< 0.05f);
}

TEST(PointOfView, getRight )
{
    PointOfView pov{
        glm::vec3(1.0, 2.0, 3.0),
        getRotatedMatrix(0.0f, glm::pi<float>() / 4.0),
        "myRoom"
    };
    auto right = pov.getRight();
    console->info("{}", vec3_to_string(right));
    ASSERT_TRUE(glm::distance(right, glm::vec3(0.7, 0.0, -0.7))< 0.05f);
}

TEST(PointOfView, getViewMatrix_id )
{
    PointOfView pov{
        glm::vec3(0.0, 0.0, 0.0),
        getRotatedMatrix(0.0f, 0.0f),
        "myRoom"
    };
    auto view = pov.getViewMatrix();
    console->info("{}", mat4x4_to_string(view));
    glm::mat4x4 expected_result(
        -1.0, 0.0, 0.0,  0.0,
        0.0,  1.0, 0.0,  0.0,
        0.0,  0.0, -1.0, 0.0,
        0.0,  0.0, 0.0,  1.0);
    ASSERT_TRUE(mat4x4_abs_diff(view, expected_result)< 0.05f);
}

TEST(PointOfView, getViewMatrix_translated )
{
    auto id = glm::mat4x4(1.0);
    PointOfView pov{
        glm::vec3(1.0, 0.0, 0.0),
        id,
        "myRoom"
    };
    auto view = pov.getViewMatrix();
    console->info("{}", mat4x4_to_string(view));
    glm::mat4x4 expected_result(
        -1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, -1.0, 0.0,
        1.0, 0.0, 0.0, 1.0);
    ASSERT_TRUE(mat4x4_abs_diff(view, expected_result)< 0.05f);
}

TEST(PointOfView, getPosMatrix )
{
    auto id = glm::mat4(1.0f);
    auto x = glm::vec3(1.0f, 0.0f , 0.0f);
    auto rot = glm::rotate(id, glm::pi<float>(), x);

    PointOfView pov{
        glm::vec3(1.0, 0.0, 0.0),
        rot,
        "myRoom"
    };
    auto mat = pov.getPosMatrix();
    console->info("{}", mat4x4_to_string(mat));
    glm::mat4x4 expected_result(
        1.0, 0.0, 0.0, 0.0,
        0.0, -1.0, 0.0, 0.0,
        0.0, 0.0, -1.0, 0.0,
        1.0, 0.0, 0.0, 1.0);
    ASSERT_TRUE(mat4x4_abs_diff(mat, expected_result)< 0.05f);
}
