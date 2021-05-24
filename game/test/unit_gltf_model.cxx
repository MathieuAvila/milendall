#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "glmock.hpp"

#include "gltf_model.hxx"

using ::testing::_;
using ::testing::InSequence;

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
