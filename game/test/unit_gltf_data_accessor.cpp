#include "catch_amalgamated.hpp"

#include <filesystem>
#include <iostream>

#include "file_library.hxx"
#include "gltf_data_accessor.hxx"
#include "gltf_exception.hxx"

using namespace nlohmann;
using namespace std;

TEST_CASE("Read range", "[gltf]" ) {

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

    CHECK(data->unit_type == GltfDataAccessorIFace::DataBlock::FLOAT);
    CHECK(data->vec_type == GltfDataAccessorIFace::DataBlock::VEC3);
    CHECK(data->count == 8);
    float* data_float = (float*)(data->data);
    CHECK(data_float[0] == 0.0);
    CHECK(data_float[1] == 0.0);
    CHECK(data_float[2] == 0.0);

    CHECK(data_float[3] == 0.0);
    CHECK(data_float[4] == 0.0);
    CHECK(data_float[5] == 1.0);

    CHECK(data_float[6] == 0.0);
    CHECK(data_float[7] == 1.0);
    CHECK(data_float[8] == 1.0);

    CHECK(data_float[9] == 0.0);
    CHECK(data_float[10] == 1.0);
    CHECK(data_float[11] == 0.0);

    CHECK(data_float[12] == 1.0);
    CHECK(data_float[13] == 0.0);
    CHECK(data_float[14] == 0.0);

    CHECK(data_float[15] == 1.0);
    CHECK(data_float[16] == 0.0);
    CHECK(data_float[17] == 1.0);

    CHECK(data_float[18] == 1.0);
    CHECK(data_float[19] == 1.0);
    CHECK(data_float[20] == 1.0);

    CHECK(data_float[21] == 1.0);
    CHECK(data_float[22] == 1.0);
    CHECK(data_float[23] == 0.0);
    }
    {
    auto data = data_accessor->accessId(1);

    CHECK(data->unit_type == GltfDataAccessorIFace::DataBlock::UNSIGNED_SHORT);
    CHECK(data->vec_type == GltfDataAccessorIFace::DataBlock::SCALAR);
    CHECK(data->count == 12);
    unsigned short* data_int = (unsigned short*)(data->data);
    CHECK(data_int[0] == 5);
    CHECK(data_int[1] == 6);
    CHECK(data_int[2] == 2);
    CHECK(data_int[3] == 5);
    CHECK(data_int[4] == 2);
    CHECK(data_int[5] == 1);
    CHECK(data_int[6] == 0);
    CHECK(data_int[7] == 3);
    CHECK(data_int[8] == 7);
    CHECK(data_int[9] == 0);
    CHECK(data_int[10] == 7);
    CHECK(data_int[11] == 4);
    }
    {
    auto data = data_accessor->accessId(2);

    CHECK(data->unit_type == GltfDataAccessorIFace::DataBlock::FLOAT);
    CHECK(data->vec_type == GltfDataAccessorIFace::DataBlock::VEC2);
    CHECK(data->count == 8);
    float* data_float = (float*)(data->data);
    CHECK(data_float[0] == 0);
    CHECK(data_float[1] == 0);

    CHECK(data_float[2] == 0);
    CHECK(data_float[3] == 0);

    CHECK(data_float[4] == 0);
    CHECK(data_float[5] == 1);

    CHECK(data_float[6] == 0);
    CHECK(data_float[7] == 1);

    CHECK(data_float[8] == 1);
    CHECK(data_float[9] == 0);

    CHECK(data_float[10] == 1);
    CHECK(data_float[11] == 0);

    CHECK(data_float[12] == 1);
    CHECK(data_float[13] == 1);

    CHECK(data_float[14] == 1);
    CHECK(data_float[15] == 1);
    }
}

