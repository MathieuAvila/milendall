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
        "myRoom"
    };
    auto rotate = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 newMatrix = glm::translate(rotate, glm::vec3(0.0f, 1.0f, 0.0f));
    auto updated = pov.changeCoordinateSystem("newRoom", newMatrix);

    console->info("{}", vec3_to_string(updated.position));
    console->info("{}", vec3_to_string(updated.up));
    console->info("{}", vec3_to_string(updated.direction));


    EXPECT_TRUE(glm::distance(updated.position, glm::vec3(-1.0f, 1.0f, 0.0f)) < 0.01f);
    EXPECT_TRUE(glm::distance(updated.up, glm::vec3(-1.0f, 0.0f, 0.0f))< 0.01f);
    EXPECT_TRUE(glm::distance(updated.direction, glm::vec3(-1.0f, 0.0f, 0.0f))< 0.01f);
    EXPECT_EQ(updated.room, "newRoom");

}

