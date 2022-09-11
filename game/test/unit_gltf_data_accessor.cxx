#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "file_library.hxx"
#include "gltf_data_accessor.hxx"
#include "gltf_exception.hxx"
#include "common.hxx"

using namespace nlohmann;
using namespace std;

static auto console = getConsole("unit_gltf_data_accessor");

#define SPECIFIC_FLOAT 3.14f
#define SPECIFIC_INT 666

static std::unique_ptr<GltfDataAccessor> get_test_accessor(std::string dir = "cube", std::string file = "room_preview.gltf")
{
    auto fl = FileLibrary();
    std::string pwd = std::filesystem::current_path();
    fl.addRootFilesystem(pwd + "/../game/test/sample/" + dir);
    auto ref = fl.getRoot().getSubPath(file);
    auto raw_json = ref.readStringContent();
    auto json_element = json::parse(raw_json.c_str());
    auto elem = std::make_unique<GltfDataAccessor>(json_element, ref.getDirPath());
    return elem;
}

static std::unique_ptr<GltfDataAccessor> get_test_accessor_blob(std::string dir, std::string file)
{
    auto fl = FileLibrary();
    std::string pwd = std::filesystem::current_path();
    fl.addRootFilesystem(pwd + "/../game/test/sample/" + dir);
    auto ref = fl.getRoot().getSubPath(file);
    auto raw_json = ref.readStringContent();
    auto json_element = json::parse(raw_json.c_str());
    FileContentPtr blob = std::make_shared<std::vector<uint8_t>>();
    for (unsigned i = 0; i < 2048; i++) {
        blob->push_back(i % 256);
    }
    // for types test, force some unsigned int and float values
    *(unsigned int*)(&blob->at(512)) = SPECIFIC_INT;
    *(float*)(&blob->at(1024)) = SPECIFIC_FLOAT;

    auto elem = std::make_unique<GltfDataAccessor>(json_element, ref.getDirPath(), blob);
    return elem;
}

TEST(GltfDataAccessor, getVec3) {

    auto data_accessor = get_test_accessor();

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

TEST(GltfDataAccessor, getScalar) {

    auto data_accessor = get_test_accessor();
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

TEST(GltfDataAccessor, getVec2) {

    auto data_accessor = get_test_accessor();
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

TEST(GltfDataAccessor, getInvalid) {

    auto data_accessor = get_test_accessor();
    EXPECT_THROW(data_accessor->accessId(42), GltfException);
}

TEST(GltfDataAccessor, getLast) {

    auto data_accessor = get_test_accessor();
    auto data = data_accessor->accessId(5);
    EXPECT_EQ(data->unit_type, GltfDataAccessorIFace::DataBlock::FLOAT);
    EXPECT_EQ(data->vec_type, GltfDataAccessorIFace::DataBlock::VEC2);
    EXPECT_EQ(data->count, 8);
}

TEST(GltfDataAccessor, data_blob) {

    auto data_accessor = get_test_accessor_blob("accessor_blob", "blob.json");
    EXPECT_NE(data_accessor, nullptr);
    auto block0 = data_accessor->accessId(0);
    EXPECT_NE(block0, nullptr);
    EXPECT_EQ(block0->count, 10);
    EXPECT_EQ(block0->data[0], 0); // 1st in blob

    auto block1 = data_accessor->accessId(1);
    EXPECT_NE(block1, nullptr);
    EXPECT_EQ(block1->count, 10);
    EXPECT_EQ(block1->data[0], 10); // offset in blob
}

TEST(GltfDataAccessor, getTypes) {

    auto data_accessor = get_test_accessor_blob("accessor_data_types", "types.json");
    EXPECT_NE(data_accessor, nullptr);

    auto block0 = data_accessor->accessId(0);
    EXPECT_NE(block0, nullptr);
    EXPECT_EQ(block0->unit_type, GltfDataAccessorIFace::DataBlock::UNSIGNED_SHORT);
    EXPECT_EQ(block0->count, 10);
    EXPECT_EQ(block0->data[0], 0);

    auto block1 = data_accessor->accessId(1);
    EXPECT_NE(block1, nullptr);
    EXPECT_EQ(block1->unit_type, GltfDataAccessorIFace::DataBlock::UNSIGNED_INT);
    EXPECT_EQ(block1->count, 10);
    unsigned int* d = (unsigned int*)(&block1->data[0]);
    EXPECT_EQ(*d, SPECIFIC_INT);

    auto block2 = data_accessor->accessId(2);
    EXPECT_NE(block2, nullptr);
    EXPECT_EQ(block2->unit_type, GltfDataAccessorIFace::DataBlock::FLOAT);
    EXPECT_EQ(block2->count, 10);
    float* d_float = (float*)(&block2->data[0]);
    EXPECT_EQ(*d_float, SPECIFIC_FLOAT); // offset in blob
}


TEST(GltfDataAccessor, stride) {

    auto data_accessor = get_test_accessor_blob("accessor_stride", "stride.json");
    EXPECT_NE(data_accessor, nullptr);

    auto block0 = data_accessor->accessId(0);
    EXPECT_NE(block0, nullptr);
    EXPECT_EQ(block0->stride, 1024);

}

TEST(GltfDataAccessor, base64) {

    auto data_accessor = get_test_accessor("accessor_base64", "sample.json");
    auto data = data_accessor->accessId(0);
    string out_s;
    for (unsigned int i = 0; i<data->size; i++)
    {
        out_s += data->data[i];
    }
    ASSERT_EQ(out_s, "THIS IS A TEST");
}
