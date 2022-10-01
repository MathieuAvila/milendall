#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nlohmann/json.hpp>

#include "room_animation.hxx"
#include "common.hxx"
#include "states_list.hxx"

#include "gltf_model.hxx"

static auto console = getConsole("unit_room_animation");

using namespace testing;
using ::testing::_;
using ::testing::InSequence;

class RoomAnimationTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_room_animation")->set_level(spdlog::level::debug);
     spdlog::get("room_animation")->set_level(spdlog::level::debug);
  }
};

class MockGltfModel : public GltfModel {
 public:

  MOCK_METHOD(void, applyAnimation, (GltfInstance* instance, std::string name,float time), (override));
  MockGltfModel() {};
  virtual ~MockGltfModel() {};

};

TEST_F(RoomAnimationTest, init_and_advance) {

    InSequence seq;

    auto json_element = json::parse(
        "{ \"name\": \"open_door\", "
        "\"false\": 0.0, "
        "\"true\": 1.0, "
        "\"event\": \"my_door_event\" "
        "}");

    StatesList states;

    auto p_model = std::make_unique<MockGltfModel>();
    MockGltfModel& model = *(p_model.get());

    auto p_instance = make_unique<GltfInstance>(model.getInstanceParameters());
    auto instance = p_instance.get();

    EXPECT_CALL(model,
        applyAnimation(instance, "open_door", NanSensitiveFloatNear(0.0, 0.01) ))
        .Times(1);
    EXPECT_CALL(model,
        applyAnimation(instance, "open_door", 0.1))
        .Times(1);
    EXPECT_CALL(model,
        applyAnimation(instance, "open_door", 0.2))
        .Times(1);
    EXPECT_CALL(model,
        applyAnimation(instance, "open_door", NanSensitiveFloatNear(1.0, 0.01)))
        .Times(2);

    EXPECT_CALL(model,
        applyAnimation(instance, "open_door", 0.9))
        .Times(1);
    EXPECT_CALL(model,
        applyAnimation(instance, "open_door", NanSensitiveFloatNear(0.8, 0.01)))
        .Times(1);
    EXPECT_CALL(model,
        applyAnimation(instance, "open_door", NanSensitiveFloatNear(0.0, 0.01)))
        .Times(2);



    RoomAnimation ra(json_element, p_model.get(), instance, &states);

    ra.animationAdvance(0.1); // FALSE. 0.0 stay
    states.setState("my_door_event", true);
    ra.animationAdvance(0.1); // TRUE. 0.1
    ra.animationAdvance(0.1); // TRUE. 0.2
    ra.animationAdvance(10.0); // TRUE. 1.0 overshoot
    ra.animationAdvance(10.0); // TRUE. 1.0 reached
    states.setState("my_door_event", false);
    ra.animationAdvance(0.1); // TRUE. 0.9
    ra.animationAdvance(0.1); // TRUE. 0.8
    ra.animationAdvance(10.0); // TRUE. 0.0 overshoot
    ra.animationAdvance(10.0); // TRUE. 0.0 reached
}

