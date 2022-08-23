#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "common.hxx"
#include "test_common.hpp"
#include "glmock.hpp"

#include "model_registry.hxx"
#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_model.hxx"

static auto console = getConsole("unit_model_registry");

using namespace testing;
using ::testing::_;
using ::testing::InSequence;

class ModelRegistryTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_model_registry")->set_level(spdlog::level::debug);
  }
};

TEST_F(ModelRegistryTest, load) {

    InSequence s;
    GLMock mock;
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    InSequence seq;

    {
        std::unique_ptr<ModelRegistry> registry = std::make_unique<ModelRegistry>(materialLibrary);

        auto fl = FileLibrary();
        fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));

        auto model1 = registry->getModel(fl.getRoot().getSubPath("/glb/pumpkin.glb"));
        ASSERT_NE(model1.get(), nullptr);
        ASSERT_EQ(2, model1.get()->getInstanceParameters());
        auto model1b = registry->getModel(fl.getRoot().getSubPath("/glb/pumpkin.glb"));
        ASSERT_EQ(model1.get(), model1b.get());
        auto model2 = registry->getModel(fl.getRoot().getSubPath("/glb/bowl.glb"));
        ASSERT_EQ(2, model2.get()->getInstanceParameters());
        ASSERT_NE(model1.get(), model2.get());
    }

}

