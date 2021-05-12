#include "catch_amalgamated.hpp"

#include <filesystem>
#include <iostream>

#include <glm/vector_relational.hpp>
#include <glm/ext/matrix_relational.hpp>

#include "common.hxx"
#include "file_library.hxx"
#include "json_helper_math.hxx"

#include "gltf_exception.hxx"

static auto console = spdlog::stdout_color_mt("unit_json_math");

TEST_CASE("Check read", "[json_matrix]" ) {

SECTION("matrix read") {
    auto json_element = json::parse("{\"matrix\": [1,2,3,4,  5,6,7,8, 9,10,11,12, 13,14,15,16]}" );
    auto json_matrix = json_element["matrix"];
    auto mat = jsonGetMatrix4x4(json_matrix);
    glm::mat4x4 truth(1,2,3,4,  5,6,7,8, 9,10,11,12, 13,14,15,16);
    REQUIRE( glm::all((glm::equal(mat, truth, 0.1f))));
}

SECTION("vec3 read") {
    auto json_element_vec3 = json::parse("{\"vec3\": [1,2,3]}" );
    auto json_vec3 = json_element_vec3["vec3"];
    auto vec3 = jsonGetVec3(json_vec3);
    glm::vec3 truth(1,2,3);
    REQUIRE( glm::all((glm::equal(vec3, truth, 0.1f))));
}

SECTION("vec4 read") {

    auto json_element_vec4 = json::parse("{\"vec4\": [1,2,3,4]}" );
    auto json_vec4 = json_element_vec4["vec4"];
    auto vec4 = jsonGetVec4(json_vec4);
    glm::vec4 truth(1,2,3,4);
    REQUIRE( glm::all((glm::equal(vec4, truth, 0.1f))));
}

SECTION("matrix fail size") {
    auto json_element = json::parse("{\"matrix\": [1,2,3,4,  5,6,7,8, 9,10,11,12, 13,14,15]}" );
    auto json_matrix = json_element["matrix"];
    REQUIRE_THROWS_AS(jsonGetMatrix4x4(json_matrix), GltfException);
}

SECTION("vec3 fail size") {
    auto json_element_vec3 = json::parse("{\"vec3\": [1,2]}" );
    auto json_vec3 = json_element_vec3["vec3"];
    REQUIRE_THROWS_AS(jsonGetVec3(json_vec3), GltfException);
}

SECTION("vec4 fail size") {
    auto json_element_vec4 = json::parse("{\"vec4\": [1,2]}" );
    auto json_vec4 = json_element_vec4["vec3"];
    REQUIRE_THROWS_AS(jsonGetVec4(json_vec4), GltfException);
}

}
