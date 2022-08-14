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


class GltfMaterialAccessorTestImpl : public GltfMaterialAccessorIFace {

    public:

        // accessing this directly in the test
        static GltfMaterialAccessorTestImpl* current;

        const FileLibrary::UriReference& dir;
        nlohmann::json file;

    GltfMaterialAccessorTestImpl(
        const FileLibrary::UriReference& _dir,
        nlohmann::json& _file) : dir(_dir), file(_file)
    {
        current = this;
    }

    void loadId(uint32_t index) override{}

    ~GltfMaterialAccessorTestImpl() override{};
};

// needed static decl
GltfMaterialAccessorTestImpl* GltfMaterialAccessorTestImpl::current;

class GltfMaterialLibraryTestImpl : public GltfMaterialLibraryIface {
public:

    //std::unique_ptr<GltfMaterialAccessorTestImpl> accessorTestImpl;

    UGltfMaterialAccessorIFace getMaterialAccessor(
        const FileLibrary::UriReference& dir,
        nlohmann::json& file) override
        {
            return std::make_unique<GltfMaterialAccessorTestImpl>(dir, file);
        }

    GltfMaterialLibraryTestImpl(){};
    virtual ~GltfMaterialLibraryTestImpl() override {};
};

TEST(GLTF_MODEL, Load_GLTF_model) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto ref = fl.getRoot().getSubPath("/cube/room_preview.gltf");
    auto materialLibrary = std::make_shared<GltfMaterialLibraryTestImpl>();
    GltfModel* model = new GltfModel(materialLibrary, ref);

    EXPECT_TRUE(model != nullptr);
}


TEST(GLTF_MODEL, Load_GLTF_model_GLB) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto ref = fl.getRoot().getSubPath("/glb/pumpkin.glb");
    auto materialLibrary = std::make_shared<GltfMaterialLibraryTestImpl>();
    GltfModel* model = new GltfModel(materialLibrary, ref);

    EXPECT_TRUE(model != nullptr);
    // High-level test to check that JSON file was loaded
    EXPECT_EQ(2, model->getInstanceParameters());
    // High-level test to check that DATA file was loaded
    EXPECT_TRUE(GltfMaterialAccessorTestImpl::current->file.contains("accessors"));
}

TEST(GLTF_MODEL, Load_GLTF_model_missing_node_name) {

    InSequence s;
    GLMock mock;

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto ref = fl.getRoot().getSubPath("/model_missing_names/room_preview.gltf");
    auto materialLibrary = std::make_shared<GltfMaterialLibraryTestImpl>();
    GltfModel* model = new GltfModel(materialLibrary, ref);

    EXPECT_TRUE(model != nullptr);
    EXPECT_EQ(model->nodeTable.size(), 6);
    auto &table = model->nodeTable;
    // next tests are for sanity checks, just to assure tests are done on the right objects.
    EXPECT_EQ(table[0].get()->name, "#0");
    EXPECT_EQ(table[1].get()->name, "#1");
    EXPECT_EQ(table[2].get()->name, "ok_2");
    EXPECT_EQ(table[3].get()->name, "ok_3");
    EXPECT_EQ(table[4].get()->name, "ok_4");
    EXPECT_EQ(table[5].get()->name, "#5");
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


