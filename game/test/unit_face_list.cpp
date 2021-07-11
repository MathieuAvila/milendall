#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "glmock.hpp"

#include "common.hxx"
#include "gltf_exception.hxx"
#include "face_list.hxx"
#include "gltf_data_accessor.hxx"

using namespace std;

static auto console = spdlog::stdout_color_mt("unit_face_list");

static const auto index_points = 21;
static const auto index_hard = 22;
static const auto index_portal = 23;

std::unique_ptr<GltfDataAccessor> get_test_face_list_accessor()
{

    auto fl = FileLibrary();
    std::string pwd = std::filesystem::current_path();
    fl.addRootFilesystem(pwd + "/../game/test/sample/face_list_sample");
    auto ref = fl.getRoot().getSubPath("sample.json");
    auto raw_json = ref.readStringContent();
    auto json_element = json::parse(raw_json.c_str());
    console->debug(to_string(json_element));
    auto elem = std::make_unique<GltfDataAccessor>(json_element, ref.getDirPath());
    return elem;
}

TEST(FaceList, Load_points) {

    auto data_accessor = get_test_face_list_accessor();
    auto data = data_accessor->accessId(index_points);

    PointsBlock block(std::move(data));
    auto points = block.getPoints();

    EXPECT_EQ(points.size(), 20);
    EXPECT_EQ(points[0], glm::vec3(-0.5, 0.0, 0.2));
    EXPECT_EQ(points[19], glm::vec3(3.0, 0.0, 0));

}

TEST(FaceList, Load_points_invalid_types) {
    auto data_accessor = get_test_face_list_accessor();
    auto data = data_accessor->accessId(index_portal);
    EXPECT_THROW(PointsBlock(std::move(data)), GltfException);
}


TEST(FaceList, Load_1_faces) {
    auto data_accessor = get_test_face_list_accessor();

    auto data_points = data_accessor->accessId(index_points);
    shared_ptr<PointsBlock> points = make_shared<PointsBlock>(move(data_points));
    auto data_portal = data_accessor->accessId(index_portal);

    FaceList faceList(points, move(data_portal));

    auto faces = faceList.getFaces();

    EXPECT_EQ(faces.size(), 1);
    EXPECT_EQ(faces.front().points.size(), 4);
    EXPECT_EQ(faces.front().points[0], 16);
    EXPECT_EQ(faces.front().points[1], 17);
    EXPECT_EQ(faces.front().points[2], 18);
    EXPECT_EQ(faces.front().points[3], 19);
}

TEST(FaceList, Load_faces_invalid_types) {

    auto data_accessor = get_test_face_list_accessor();

    auto data_points = data_accessor->accessId(index_points);
    shared_ptr<PointsBlock> points = make_shared<PointsBlock>(move(data_points));
    auto data_portal = data_accessor->accessId(index_points); // invalid to point to float

    EXPECT_THROW(FaceList faceList(points, move(data_portal)), GltfException);

}

TEST(FaceList, Load__multi_faces) {
    auto data_accessor = get_test_face_list_accessor();

    auto data_points = data_accessor->accessId(index_points);
    shared_ptr<PointsBlock> points = make_shared<PointsBlock>(move(data_points));
    auto data_portal = data_accessor->accessId(index_hard);

    FaceList faceList(points, move(data_portal));

    auto faces = faceList.getFaces();

    // check number of faces
    EXPECT_EQ(faces.size(), 12);

    // check first face
    EXPECT_EQ(faces.front().points.size(), 4);
    EXPECT_EQ(faces.front().points[0], 0);
    EXPECT_EQ(faces.front().points[1], 1);
    EXPECT_EQ(faces.front().points[2], 2);
    EXPECT_EQ(faces.front().points[3], 7);

    // check last face
    EXPECT_EQ(faces.back().points.size(), 4);
    EXPECT_EQ(faces.back().points[0], 15);
    EXPECT_EQ(faces.back().points[1], 8);
    EXPECT_EQ(faces.back().points[2], 0);
    EXPECT_EQ(faces.back().points[3], 7);
}