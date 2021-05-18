#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include <glm/vector_relational.hpp>
#include <glm/ext/matrix_relational.hpp>

#include "common.hxx"
#include "file_library.hxx"
#include "json_helper_math.hxx"

#include "gltf_exception.hxx"

static auto console = spdlog::stdout_color_mt("unit_json_math");

TEST(JSON_HELPER_MATH, matrix_read) {
    auto json_element = json::parse("{\"matrix\": [1,2,3,4,  5,6,7,8, 9,10,11,12, 13,14,15,16]}" );
    auto json_matrix = json_element["matrix"];
    auto mat = jsonGetMatrix4x4(json_matrix);
    glm::mat4x4 truth(1,2,3,4,  5,6,7,8, 9,10,11,12, 13,14,15,16);
    EXPECT_TRUE( glm::all((glm::equal(mat, truth, 0.1f))));
}

TEST(JSON_HELPER_MATH, vec3_read) {
    auto json_element_vec3 = json::parse("{\"vec3\": [1,2,3]}" );
    auto json_vec3 = json_element_vec3["vec3"];
    auto vec3 = jsonGetVec3(json_vec3);
    glm::vec3 truth(1,2,3);
    EXPECT_TRUE( glm::all((glm::equal(vec3, truth, 0.1f))));
}

TEST(JSON_HELPER_MATH, vec4_read) {
    auto json_element_vec4 = json::parse("{\"vec4\": [1,2,3,4]}" );
    auto json_vec4 = json_element_vec4["vec4"];
    auto vec4 = jsonGetVec4(json_vec4);
    glm::vec4 truth(1,2,3,4);
    EXPECT_TRUE( glm::all((glm::equal(vec4, truth, 0.1f))));
}

TEST(JSON_HELPER_MATH, matrix_fail_size) {
    auto json_element = json::parse("{\"matrix\": [1,2,3,4,  5,6,7,8, 9,10,11,12, 13,14,15]}" );
    auto json_matrix = json_element["matrix"];
    EXPECT_THROW(jsonGetMatrix4x4(json_matrix), GltfException);
}

TEST(JSON_HELPER_MATH, vec3_fail_size) {
    auto json_element_vec3 = json::parse("{\"vec3\": [1,2]}" );
    auto json_vec3 = json_element_vec3["vec3"];
    EXPECT_THROW(jsonGetVec3(json_vec3), GltfException);
}

TEST(JSON_HELPER_MATH, vec4_fail_size) {
    auto json_element_vec4 = json::parse("{\"vec4\": [1,2]}" );
    auto json_vec4 = json_element_vec4["vec3"];
    EXPECT_THROW(jsonGetVec4(json_vec4), GltfException);
}

