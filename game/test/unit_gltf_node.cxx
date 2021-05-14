#include "catch_amalgamated.hpp"

#include <filesystem>
#include <iostream>

#include "common.hxx"
#include "gltf_model.hxx"
#include "helper_math.hxx"

#include <glm/mat4x4.hpp>
#include <glm/vector_relational.hpp>
#include <glm/ext/matrix_relational.hpp>

static auto console = spdlog::stdout_color_mt("ut");

TEST_CASE( "Load GLTF node", "[gltf-node]" ) {

static glm::mat4x4 identity = glm::mat4(1.0f);

SECTION("Load void")
{
    auto json_node = json::parse(
        "{ }" );
    GltfNode node(json_node);

    REQUIRE(node.children == GltfNode::ChildrenList{});
    REQUIRE(glm::all((glm::equal(node.default_transform, identity))));
    REQUIRE(node.my_mesh == -1);
}


SECTION("Load children and matrix and mesh")
{
    auto json_node = json::parse(
        "{ \
            \"children\": [\
                1,2,3,4\
            ],\
            \"mesh\" : 10,\
            \"matrix\": [ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]}" );
    GltfNode node(json_node);

    REQUIRE(node.children == GltfNode::ChildrenList{1,2,3,4});
    REQUIRE(glm::all((glm::equal(node.default_transform, glm::mat4x4(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15)))));
    REQUIRE(node.my_mesh == 10);
}


SECTION("Load children and translation")
{
    auto json_node = json::parse(
        "{ \
            \"children\": [\
                1,2,3,4\
            ],\
            \"translation\": [10,11,12]}" );
    GltfNode node(json_node);

    REQUIRE(node.children == GltfNode::ChildrenList{1,2,3,4});

    console->debug(mat4x4_to_string(glm::transpose(node.default_transform)));

    REQUIRE(glm::all((glm::equal(node.default_transform, glm::mat4x4(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        10,11,12,1
        )))));
}


}
