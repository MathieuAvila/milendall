#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <filesystem>
#include <iostream>

#include "common.hxx"
#include "gltf_animation.hxx"
#include "gltf_data_accessor.hxx"
#include "helper_math.hxx"
#include "json_helper_accessor.hxx"
#include <glm/ext/matrix_transform.hpp>

using namespace testing;
using ::testing::_;

static auto console = getConsole("ut_animation");

class GltfAnimationTest : public ::testing::Test {
 protected:

    std::unique_ptr<GltfDataAccessor> data_accessor;
    nlohmann::json json_element;

  void SetUp() override
  {
    spdlog::get("ut_animation")->set_level(spdlog::level::debug);
    spdlog::get("gltf_animation")->set_level(spdlog::level::debug);

    auto fl = FileLibrary();
    std::string pwd = std::filesystem::current_path();
    fl.addRootFilesystem(pwd + "/../game/test/sample/animation_simple");
    auto ref = fl.getRoot().getSubPath("room_preview.gltf");
    auto raw_json = ref.readStringContent();
    json_element = json::parse(raw_json.c_str());
    data_accessor = std::make_unique<GltfDataAccessor>(json_element, ref.getDirPath());
  }

  nlohmann::json getJsonAnimation(int index)
  {
    return jsonGetElementByIndex(json_element, "animations", index);
  }
};

TEST_F(GltfAnimationTest, load) {
    nlohmann::json json = getJsonAnimation(0);
    GltfAnimation animation(json, data_accessor.get());
    // Expect no throw
}

class MockGltfAnimationTargetIface : public GltfAnimationTargetIface {
 public:

  MOCK_METHOD(void, applyChange, (int index, glm::mat4x4 matrix), (override));
};

TEST_F(GltfAnimationTest, apply_translation_first) {
    nlohmann::json json = getJsonAnimation(0);
    GltfAnimation animation(json, data_accessor.get());
    // Expect no throw

    MockGltfAnimationTargetIface instance;
    EXPECT_CALL(instance,
        applyChange(4, glm::mat4x4(1.0f)))
        .WillOnce(Return());

    animation.apply(0.0, &instance);
}

TEST_F(GltfAnimationTest, apply_translation_intermediate) {
    nlohmann::json json = getJsonAnimation(0);
    GltfAnimation animation(json, data_accessor.get());
    // Expect no throw

    MockGltfAnimationTargetIface instance;
    //translate half
    glm::mat4x4 mat_translate = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f, -0.75f, 0.0f));

    EXPECT_CALL(instance,
        applyChange(4, mat_translate))
        .WillOnce(Return());

    animation.apply(0.45, &instance);
}

TEST_F(GltfAnimationTest, apply_translation_final) {
    nlohmann::json json = getJsonAnimation(0);
    GltfAnimation animation(json, data_accessor.get());
    // Expect no throw

    MockGltfAnimationTargetIface instance;
    //translate full
    glm::mat4x4 mat_translate = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f, -1.5f, 0.0f));

    EXPECT_CALL(instance,
        applyChange(4, mat_translate))
        .WillOnce(Return());

    animation.apply(0.9, &instance);
}

TEST_F(GltfAnimationTest, apply_translation_PAST_final) {
    nlohmann::json json = getJsonAnimation(0);
    GltfAnimation animation(json, data_accessor.get());
    // Expect no throw

    MockGltfAnimationTargetIface instance;
    //translate half
    glm::mat4x4 mat_translate = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f, -1.5f, 0.0f));

    EXPECT_CALL(instance,
        applyChange(4, mat_translate))
        .WillOnce(Return());

    animation.apply(1.0, &instance);
}

