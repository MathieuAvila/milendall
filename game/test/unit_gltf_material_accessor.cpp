#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "file_library.hxx"
#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_exception.hxx"
#include "glmock.hpp"

#include "test_common.hpp"

using namespace nlohmann;
using namespace std;
using namespace testing;
using ::testing::_;
using ::testing::InSequence;

TEST(GltfMaterial, ReadValidMaterial)
{

    InSequence s;
    GLMock mock;

    EXPECT_CALL(mock, gl_GenTextures(1, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_GetError()).WillOnce(Return(0));
    EXPECT_CALL(mock, gl_BindTexture(GL_TEXTURE_2D, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_GetError()).WillOnce(Return(0));
    EXPECT_CALL(mock, gl_TexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_GetError()).WillOnce(Return(0));
    EXPECT_CALL(mock, gl_TexParameteri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, gl_GetError()).WillOnce(Return(0));

    EXPECT_CALL(mock, gl_GenTextures(1, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_GetError()).WillOnce(Return(0));
    EXPECT_CALL(mock, gl_BindTexture(GL_TEXTURE_2D, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_GetError()).WillOnce(Return(0));
    EXPECT_CALL(mock, gl_TexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, _)).WillOnce(Return());
    EXPECT_CALL(mock, gl_GetError()).WillOnce(Return(0));
    EXPECT_CALL(mock, gl_TexParameteri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, gl_GetError()).WillOnce(Return(0));

    EXPECT_CALL(mock, gl_DeleteTextures(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, gl_DeleteTextures(_, _)).WillRepeatedly(Return());

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto material_uri = fl.getRoot().getSubPath("material/valid_3_3_3.gltf");
    auto json_content = json::parse(material_uri.readStringContent());

    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto accessor = materialLibrary->getMaterialAccessor(material_uri.getDirPath(), json_content);
}

TEST(GltfMaterial, use_material)
{
    InSequence s;
    GLMock mock;

    EXPECT_CALL(mock, gl_TexParameteri(_, _, _)).WillRepeatedly(Return());

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto material_uri = fl.getRoot().getSubPath("material/material_pbr_color_texture.gltf");
    auto json_content = json::parse(material_uri.readStringContent());

    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto accessor = materialLibrary->getMaterialAccessor(material_uri.getDirPath(), json_content);

    accessor->loadId(0);
    EXPECT_EQ(isCurrentModeTexture, false);
    EXPECT_EQ(currentColor[0], 0.1f);
    EXPECT_EQ(currentColor[1], 0.2f);
    EXPECT_EQ(currentColor[2], 0.3f);
    EXPECT_EQ(currentMetallicFactor, 0.5f);
    EXPECT_EQ(currentRoughnessFactor, 0.6f);

    accessor->loadId(1);
    EXPECT_EQ(isCurrentModeTexture, false);
    EXPECT_EQ(currentColor[0], 0.1f);
    EXPECT_EQ(currentColor[1], 0.2f);
    EXPECT_EQ(currentColor[2], 0.3f);
    EXPECT_EQ(currentMetallicFactor, 1.0f);
    EXPECT_EQ(currentRoughnessFactor, 1.0f);

    accessor->loadId(2);
    EXPECT_EQ(isCurrentModeTexture, true);
    //EXPECT_EQ(currentTexture, 0);  TODO: generate a specific texture ID value
    EXPECT_EQ(currentMetallicFactor, 0.5f);
    EXPECT_EQ(currentRoughnessFactor, 0.6f);

    accessor->loadId(3);
    EXPECT_EQ(isCurrentModeTexture, true);
    //EXPECT_EQ(currentTexture, 0);  TODO: generate a specific texture ID value
    EXPECT_EQ(currentMetallicFactor, 1.0f);
    EXPECT_EQ(currentRoughnessFactor, 1.0f);
}