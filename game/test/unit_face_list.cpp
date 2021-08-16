#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "glmock.hpp"

#include "common.hxx"
#include "gltf/gltf_exception.hxx"
#include "face_list.hxx"
#include "gltf/gltf_data_accessor.hxx"

#include <glm/gtx/string_cast.hpp>

using namespace std;

static auto console = spdlog::stdout_color_mt("unit_face_list");

static const auto index_points = 21;
static const auto index_hard = 22;
static const auto index_portal = 23;

static const auto index_points_room_ground = 9;
static const auto index_faces_room_ground = 10;

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

TEST(FaceList, PointsBlock_Load_points) {

    auto data_accessor = get_test_face_list_accessor();
    auto data = data_accessor->accessId(index_points);

    PointsBlock block(std::move(data));
    auto points = block.getPoints();

    EXPECT_EQ(points.size(), 20);
    EXPECT_EQ(points[0], glm::vec3(-0.5, 0.0, 0.2));
    EXPECT_EQ(points[19], glm::vec3(3.0, 0.0, 0));

}

TEST(FaceList, PointsBlock_Load_points_invalid_types) {
    auto data_accessor = get_test_face_list_accessor();
    auto data = data_accessor->accessId(index_portal);
    EXPECT_THROW(PointsBlock(std::move(data)), GltfException);
}


TEST(FaceList, FaceList_Load_1_faces) {
    auto data_accessor = get_test_face_list_accessor();

    auto data_points = data_accessor->accessId(index_points);
    shared_ptr<PointsBlock> points = make_shared<PointsBlock>(move(data_points));
    auto data_portal = data_accessor->accessId(index_portal);

    FaceList faceList(points, move(data_portal));

    auto faces = faceList.getFaces();

    EXPECT_EQ(faces.size(), 1);
    EXPECT_EQ(faces.front().indices.size(), 4);
    EXPECT_EQ(faces.front().indices[0].index, 16);
    EXPECT_EQ(faces.front().indices[1].index, 17);
    EXPECT_EQ(faces.front().indices[2].index, 18);
    EXPECT_EQ(faces.front().indices[3].index, 19);
}

TEST(FaceList, FaceList_Load_faces_invalid_types) {

    auto data_accessor = get_test_face_list_accessor();

    auto data_points = data_accessor->accessId(index_points);
    shared_ptr<PointsBlock> points = make_shared<PointsBlock>(move(data_points));
    auto data_portal = data_accessor->accessId(index_points); // invalid to point to float

    EXPECT_THROW(FaceList faceList(points, move(data_portal)), GltfException);

}

TEST(FaceList, FaceList_Load_multi_faces) {
    auto data_accessor = get_test_face_list_accessor();

    auto data_points = data_accessor->accessId(index_points);
    shared_ptr<PointsBlock> points = make_shared<PointsBlock>(move(data_points));
    auto data_portal = data_accessor->accessId(index_hard);

    FaceList faceList(points, move(data_portal));

    auto faces = faceList.getFaces();

    // check number of faces
    EXPECT_EQ(faces.size(), 12);

    // check first face
    EXPECT_EQ(faces.front().indices.size(), 4);
    EXPECT_EQ(faces.front().indices[0].index, 0);
    EXPECT_EQ(faces.front().indices[1].index, 1);
    EXPECT_EQ(faces.front().indices[2].index, 2);
    EXPECT_EQ(faces.front().indices[3].index, 7);

    // check last face
    EXPECT_EQ(faces.back().indices.size(), 4);
    EXPECT_EQ(faces.back().indices[0].index, 15);
    EXPECT_EQ(faces.back().indices[1].index, 8);
    EXPECT_EQ(faces.back().indices[2].index, 0);
    EXPECT_EQ(faces.back().indices[3].index, 7);
}

void check_normal_plane(
    GltfDataAccessor* data_accessor,
    shared_ptr<PointsBlock> points,
    int index,
    glm::vec3 normal,
    glm::vec4 plane)
{
        auto data_portal = data_accessor->accessId(index);
        FaceList faceList(points, move(data_portal));
        auto faces = faceList.getFaces();
        auto& f = faces.front();
        console->info("{}", glm::to_string(f.normal));
        console->info("{}", glm::to_string(f.plane));
        EXPECT_EQ(f.normal, normal);
        EXPECT_EQ(f.plane, plane);
}

TEST(FaceList, FaceList_normals_planes) {

    auto data_accessor = get_test_face_list_accessor();
    auto data_points = data_accessor->accessId(9);
    shared_ptr<PointsBlock> points = make_shared<PointsBlock>(move(data_points));

    // ground : up
    check_normal_plane(data_accessor.get(), points, 10,
        glm::vec3(0,  1.0,  0),
        glm::vec4(0,  1.0,  0, 0.0)
        );
    // ceiling : down
    check_normal_plane(data_accessor.get(), points, 11,
        glm::vec3(0,  -1.0,   0),
        glm::vec4(0,  -1.0,  0, 4.5)
        );
    // first wall : horizontal
    check_normal_plane(data_accessor.get(), points, 12,
        glm::vec3(0,  0,  1.0),
        glm::vec4(0,  0,  1.0 , 0.0)
        );
}

std::list<FaceList::Face> getGroundFaces()
{
    auto data_accessor = get_test_face_list_accessor();
    auto data_points = data_accessor->accessId(index_points_room_ground);
    shared_ptr<PointsBlock> points = make_shared<PointsBlock>(move(data_points));
    auto data_portal = data_accessor->accessId(index_faces_room_ground);
    FaceList faceList(points, move(data_portal));
    return faceList.getFaces();
}

TEST(FaceList, FaceList_normals_planes_for_each_point) {

    // will check normals computations for each point

    auto faces = getGroundFaces();

    EXPECT_EQ(faces.size(), 1);
    EXPECT_EQ(faces.front().indices.size(), 4);
    EXPECT_EQ(faces.front().indices[0].plane, glm::vec4(0,0,-1,13.5));
    EXPECT_EQ(faces.front().indices[1].plane, glm::vec4(-1,0,0,4));
    EXPECT_EQ(faces.front().indices[2].plane, glm::vec4(0,0,1,0));
    EXPECT_EQ(faces.front().indices[3].plane, glm::vec4(1,0,0,0));
}

TEST(FaceList, FaceList_check_point_inside_space) {

    // will check computing if a point is inside space defined by points normals
    // will check normals computations for each point

    auto faces = getGroundFaces();
    EXPECT_EQ(faces.size(), 1);
    auto& face = faces.front();
    ASSERT_TRUE(face.checkInVolume(glm::vec3(1.0, 0.0, 1.0) )); // center, same level
    ASSERT_TRUE(face.checkInVolume(glm::vec3(1.0, 1000.0, 1.0) )); // center, up
    ASSERT_TRUE(face.checkInVolume(glm::vec3(1.0, -1000.0, 1.0) )); // center, down

    ASSERT_FALSE(face.checkInVolume(glm::vec3(5.0, 0.0, 1.0) )); // outside first border
    ASSERT_FALSE(face.checkInVolume(glm::vec3(5.0, 0.0, 1.0) )); // outside 2nd border
    ASSERT_FALSE(face.checkInVolume(glm::vec3(0.0, 0.0, -1.0) )); // outside 4th border
}

TEST(FaceList, FaceList_trajectory_ok) {

    // will check trajectory intersection is fine
    auto faces = getGroundFaces();
    auto& face = faces.front();
    glm::vec3 impact;
    float distance;
    glm::vec3 normal;

    ASSERT_TRUE(face.checkTrajectoryCross(
        glm::vec3(1.0, 1000.0, 1.0), glm::vec3(1.0, -1000.0, 1.0),
        impact, distance, normal));
    ASSERT_EQ(impact, glm::vec3(1.0, 0.0, 1.0));
    ASSERT_EQ(distance, 1000.0f);
    ASSERT_EQ(normal, glm::vec3(0.0, 1.0, 0.0));
}

TEST(FaceList, FaceList_trajectory_bad_direction) {

    // will check trajectory crosses face in bad direction

    auto faces = getGroundFaces();
    auto& face = faces.front();
    glm::vec3 impact;
    float distance;
    glm::vec3 normal;

    ASSERT_FALSE(face.checkTrajectoryCross(
        glm::vec3(1.0, -1000.0, 1.0), glm::vec3(1.0, 1000.0, 1.0),
        impact, distance, normal));
}

TEST(FaceList, FaceList_trajectory_bad_direction_reversed) {

    // will check trajectory crosses face in bad direction, but in reversed mode that is OK

    auto faces = getGroundFaces();
    auto& face = faces.front();
    glm::vec3 impact;
    float distance;
    glm::vec3 normal;

    ASSERT_TRUE(face.checkTrajectoryCross(
        glm::vec3(1.0, -1000.0, 1.0), glm::vec3(1.0, 1000.0, 1.0),
        impact, distance, normal, true));
}

TEST(FaceList, FaceList_trajectory_doesnt_touch) {

    // will check trajectory doesn't reach surface

    auto faces = getGroundFaces();
    auto& face = faces.front();
    glm::vec3 impact;
    float distance;
    glm::vec3 normal;

    ASSERT_FALSE(face.checkTrajectoryCross(
        glm::vec3(1.0, 1000.0, 1.0), glm::vec3(1.0, 10.0, 1.0), // 10.0 more than surface.
        impact, distance, normal));
}

TEST(FaceList, FaceList_trajectory_borders) {

    // will check trajectory doesn't reach surface

    auto faces = getGroundFaces();
    auto& face = faces.front();
    glm::vec3 impact;
    float distance;
    glm::vec3 normal;

    // 1st border
    ASSERT_TRUE(face.checkSphereTrajectoryCross(
        glm::vec3(-1.0, 1000.0, 1.0), glm::vec3(-1.0, -1000.0, 1.0), // should hit at 1.0
        2.0,
        impact, distance, normal));
    console->info("{}", glm::to_string(impact));
    console->info("{}", distance);
    console->info("{}", glm::to_string(normal));

     // 3th border
    ASSERT_TRUE(face.checkSphereTrajectoryCross(
        glm::vec3(-1.0, 1000.0, 14.0), glm::vec3(-1.0, -1000.0, 14.0), // should hit at 1.0
        2.0,
        impact, distance, normal));
    console->info("{}", glm::to_string(impact));
    console->info("{}", distance);
    console->info("{}", glm::to_string(normal));


}