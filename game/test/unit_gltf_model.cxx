#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "glmock.hpp"

#include "gltf/gltf_model.hxx"

using ::testing::_;
using ::testing::InSequence;
using namespace std;

class GltfModelTest : public GltfModel
{
    public:

        using GltfModel::GltfModel;

        auto TEST_getRootNodes() {
            return root_nodes;
        }
};

TEST(GLTF_MODEL, Load_GLTF_model) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto ref = fl.getRoot().getSubPath("/cube/room_preview.gltf");
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    GltfModel* model = new GltfModel(materialLibrary, ref);

    EXPECT_TRUE(model != nullptr);
}

TEST(GLTF_MODEL, multi_scene_multi_children_matrices) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto ref = fl.getRoot().getSubPath("/model_multi_node_no_mesh/room_preview.gltf");
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    GltfModelTest* model = new GltfModelTest(materialLibrary, ref);

    EXPECT_TRUE(model != nullptr);

    // check root nodes
    EXPECT_EQ(model->TEST_getRootNodes(), (std::vector<int>{1,2}));
    EXPECT_EQ(model->getInstanceParameters(), (6));

    // apply transform : check matrices
    auto instance = make_unique<GltfInstance>(model->getInstanceParameters());
    EXPECT_TRUE(instance != nullptr);
    auto mat_3 = glm::mat4(3.0);
    model->applyDefaultTransform(instance.get(), mat_3);

    EXPECT_EQ(instance->getNode(0)->getNodeMatrix(), glm::mat4(1.0)); // unused

    EXPECT_EQ(instance->getNode(1)->getNodeMatrix()[0][0], (3.0 * 2.0) ); // pos * root 1
    EXPECT_EQ(instance->getNode(2)->getNodeMatrix()[0][0], (3.0 * 1.0) ); // pos * root 2

    EXPECT_EQ(instance->getNode(3)->getNodeMatrix()[0][0], (3.0 * 2.0) );
    EXPECT_EQ(instance->getNode(4)->getNodeMatrix()[0][0], (3.0 * 2.0 * 3.0) );

    EXPECT_EQ(instance->getNode(5)->getNodeMatrix()[0][0], (3.0 * 2.0 * 3.0 * 3.0) );

}


TEST(GLTF_MODEL, draw_multi_scene_multi_children_matrices) {

}

TEST(GLTF_MODEL, animation) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto ref = fl.getRoot().getSubPath("/room_2_animations/room_preview.gltf");
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    GltfModelTest* model = new GltfModelTest(materialLibrary, ref);

    auto instance = make_unique<GltfInstance>(model->getInstanceParameters());
    model->applyAnimation(instance.get(), "open_door1", 0.1);
    model->applyAnimation(instance.get(), "open_door2", 0.1);
}


