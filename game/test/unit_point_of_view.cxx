#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "common.hxx"
#include <point_of_view.hxx>
#include <glm/gtc/matrix_transform.hpp>
#include "helper_math.hxx"

static auto console = spdlog::stdout_color_mt("ut_pov");

TEST(PointOfView, changeCoordinateSystem)
{
    PointOfView pov{
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::mat3x3(1.0f),
        "myRoom"
    };
    auto rotate = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 newMatrix = glm::translate(rotate, glm::vec3(0.0f, 1.0f, 0.0f));
    auto updated = pov.changeCoordinateSystem("newRoom", newMatrix);

    console->info("{}", to_string(updated));

    EXPECT_TRUE(glm::distance(updated.position, glm::vec3(-1.0f, 1.0f, 0.0f)) < 0.01f);
    EXPECT_TRUE(glm::distance(updated.up, glm::vec3(-1.0f, 0.0f, 0.0f))< 0.01f);
    EXPECT_TRUE(glm::distance(updated.direction, glm::vec3(-1.0f, 0.0f, 0.0f))< 0.01f);
    EXPECT_EQ(updated.room, "newRoom");
}

TEST(PointOfView, initAngles)
{
    PointOfView pov{
        glm::vec3(1.0, 2.0, 3.0),
        glm::pi<float>()/8.0f,
        glm::pi<float>()/4.0f,
        glm::mat3x3(2.0f),
        "myRoom"
    };
    console->info("{}", to_string(pov));
    EXPECT_TRUE(glm::distance(pov.position, glm::vec3(1.0f, 2.0f, 3.0f)) < 0.05f);
    EXPECT_TRUE(glm::distance(pov.direction, glm::vec3( 1.30, 0.76, 1.30))< 0.05f);
    EXPECT_TRUE(glm::distance(pov.up, glm::vec3(-0.54, 1.84, -0.54))< 0.05f);
    EXPECT_EQ(glm::mat3x3(2.0f), pov.local_reference);
    EXPECT_EQ(pov.room, "myRoom");
}
