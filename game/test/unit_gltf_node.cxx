#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "common.hxx"
#include "gltf/gltf_model.hxx"
#include "helper_math.hxx"

#include <glm/mat4x4.hpp>
#include <glm/vector_relational.hpp>
#include <glm/ext/matrix_relational.hpp>

static auto console = spdlog::stdout_color_mt("ut");

static glm::mat4x4 identity = glm::mat4(1.0f);

TEST(GLTF_NODE, Load_void)
{
    auto json_node = json::parse(
        "{ }" );
    GltfNode node(json_node);

    EXPECT_TRUE(node.children == GltfNode::ChildrenList{});
    EXPECT_TRUE(glm::all((glm::equal(node.default_transform, identity))));
    EXPECT_TRUE(node.my_mesh == -1);
}


TEST(GLTF_NODE, Load_children_and_matrix_and_mesh)
{
    auto json_node = json::parse(
        "{ \
            \"children\": [\
                1,2,3,4\
            ],\
            \"mesh\" : 10,\
            \"matrix\": [ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]}" );
    GltfNode node(json_node);

    EXPECT_TRUE( (node.children == GltfNode::ChildrenList{1,2,3,4}) );
    EXPECT_TRUE(glm::all((glm::equal(node.default_transform, glm::mat4x4(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15)))));
    EXPECT_TRUE(node.my_mesh == 10);
}


TEST(GLTF_NODE, Load_children_and_translation)
{
    auto json_node = json::parse(
        "{ \
            \"children\": [\
                1,2,3,4\
            ],\
            \"translation\": [10,11,12]}" );
    GltfNode node(json_node);

    EXPECT_TRUE((node.children == GltfNode::ChildrenList{1,2,3,4}));

    console->debug(mat4x4_to_string(glm::transpose(node.default_transform)));

    EXPECT_TRUE(glm::all((glm::equal(node.default_transform, glm::mat4x4(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        10,11,12,1
        )))));
}

