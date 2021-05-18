#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "file_library.hxx"
#include "gltf_data_accessor.hxx"
#include "gltf_exception.hxx"

using namespace nlohmann;
using namespace std;

TEST(GLTF_DATA_ACCESSOR, GLTF_DATA_ACCESSOR) {

    auto fl = FileLibrary();
    std::string pwd = std::filesystem::current_path();
    fl.addRootFilesystem(pwd + "/../game/test/sample/cube");
    auto ref = fl.getRoot().getSubPath("room_preview.gltf");
    auto raw_json = ref.readStringContent();
    auto json_element = json::parse(raw_json.c_str());

    //unique_ptr<GltfDataAccessorIFace> data_accessor(new GltfDataAccessor(json_element, ref.getDirPath()));

    auto data_accessor = std::make_unique<GltfDataAccessor>(json_element, ref.getDirPath());
    {
    auto data = data_accessor->accessId(0);

    EXPECT_TRUE(data->unit_type == GltfDataAccessorIFace::DataBlock::FLOAT);
    EXPECT_TRUE(data->vec_type == GltfDataAccessorIFace::DataBlock::VEC3);
    EXPECT_TRUE(data->count == 8);
    float* data_float = (float*)(data->data);
    EXPECT_TRUE(data_float[0] == 0.0);
    EXPECT_TRUE(data_float[1] == 0.0);
    EXPECT_TRUE(data_float[2] == 0.0);

    EXPECT_TRUE(data_float[3] == 0.0);
    EXPECT_TRUE(data_float[4] == 0.0);
    EXPECT_TRUE(data_float[5] == 1.0);

    EXPECT_TRUE(data_float[6] == 0.0);
    EXPECT_TRUE(data_float[7] == 1.0);
    EXPECT_TRUE(data_float[8] == 1.0);

    EXPECT_TRUE(data_float[9] == 0.0);
    EXPECT_TRUE(data_float[10] == 1.0);
    EXPECT_TRUE(data_float[11] == 0.0);

    EXPECT_TRUE(data_float[12] == 1.0);
    EXPECT_TRUE(data_float[13] == 0.0);
    EXPECT_TRUE(data_float[14] == 0.0);

    EXPECT_TRUE(data_float[15] == 1.0);
    EXPECT_TRUE(data_float[16] == 0.0);
    EXPECT_TRUE(data_float[17] == 1.0);

    EXPECT_TRUE(data_float[18] == 1.0);
    EXPECT_TRUE(data_float[19] == 1.0);
    EXPECT_TRUE(data_float[20] == 1.0);

    EXPECT_TRUE(data_float[21] == 1.0);
    EXPECT_TRUE(data_float[22] == 1.0);
    EXPECT_TRUE(data_float[23] == 0.0);
    }
    {
    auto data = data_accessor->accessId(1);

    EXPECT_TRUE(data->unit_type == GltfDataAccessorIFace::DataBlock::UNSIGNED_SHORT);
    EXPECT_TRUE(data->vec_type == GltfDataAccessorIFace::DataBlock::SCALAR);
    EXPECT_TRUE(data->count == 12);
    unsigned short* data_int = (unsigned short*)(data->data);
    EXPECT_TRUE(data_int[0] == 5);
    EXPECT_TRUE(data_int[1] == 6);
    EXPECT_TRUE(data_int[2] == 2);
    EXPECT_TRUE(data_int[3] == 5);
    EXPECT_TRUE(data_int[4] == 2);
    EXPECT_TRUE(data_int[5] == 1);
    EXPECT_TRUE(data_int[6] == 0);
    EXPECT_TRUE(data_int[7] == 3);
    EXPECT_TRUE(data_int[8] == 7);
    EXPECT_TRUE(data_int[9] == 0);
    EXPECT_TRUE(data_int[10] == 7);
    EXPECT_TRUE(data_int[11] == 4);
    }
    {
    auto data = data_accessor->accessId(2);

    EXPECT_TRUE(data->unit_type == GltfDataAccessorIFace::DataBlock::FLOAT);
    EXPECT_TRUE(data->vec_type == GltfDataAccessorIFace::DataBlock::VEC2);
    EXPECT_TRUE(data->count == 8);
    float* data_float = (float*)(data->data);
    EXPECT_TRUE(data_float[0] == 0);
    EXPECT_TRUE(data_float[1] == 0);

    EXPECT_TRUE(data_float[2] == 0);
    EXPECT_TRUE(data_float[3] == 0);

    EXPECT_TRUE(data_float[4] == 0);
    EXPECT_TRUE(data_float[5] == 1);

    EXPECT_TRUE(data_float[6] == 0);
    EXPECT_TRUE(data_float[7] == 1);

    EXPECT_TRUE(data_float[8] == 1);
    EXPECT_TRUE(data_float[9] == 0);

    EXPECT_TRUE(data_float[10] == 1);
    EXPECT_TRUE(data_float[11] == 0);

    EXPECT_TRUE(data_float[12] == 1);
    EXPECT_TRUE(data_float[13] == 1);

    EXPECT_TRUE(data_float[14] == 1);
    EXPECT_TRUE(data_float[15] == 1);
    }
}

