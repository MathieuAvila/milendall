#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "file_library.hxx"
#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_exception.hxx"
#include "glmock.hpp"

using namespace nlohmann;
using namespace std;
using namespace testing;
using ::testing::_;
using ::testing::InSequence;

TEST(GltfMaterial, ReadValidMaterial ) {

    InSequence s;
    GLMock mock;

    EXPECT_CALL(mock, gl_GenTextures(1, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_BindTexture(GL_TEXTURE_2D, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_BYTE,  GL_RGB, _)).WillOnce(Return());

    EXPECT_CALL(mock, gl_GenTextures(1, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_BindTexture(GL_TEXTURE_2D, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_BYTE,  GL_RGB, _)).WillOnce(Return());

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto material_uri = fl.getRoot().getSubPath("material/valid_3_3_3.gltf");
    auto json_content = json::parse(material_uri.readStringContent());

    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto accessor = materialLibrary->getMaterialAccessor(material_uri.getDirPath(), json_content);

}




