#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "common.hxx"
#include "managed_object_instance.hxx"
#include "gravity_provider.hxx"

#include <glm/gtx/string_cast.hpp>


using namespace testing;
using ::testing::_;

static auto console = getConsole("test_managed_object_instance");

class ManagedObjectInstanceTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("managed_object_instance")->set_level(spdlog::level::debug);
     spdlog::get("test_managed_object_instance")->set_level(spdlog::level::debug);
  }
};

TEST_F(ManagedObjectInstanceTest, advance_0_dont_move) {


}

class MockGravityProvider : public GravityProvider {
 public:

  MOCK_METHOD(GravityInformation, getGravityInformation, (const PointOfView& position,
            glm::vec3 speed,
            float weight,
            float radius,
            float total_time), (const, override));
};

class FakeObject: public ManagedObject
{
    public:
    MovableObjectDefinition def;
    virtual MovableObjectDefinition& getObjectDefinition() {
        return def;
    };
    virtual MovementWish getRequestedMovement() { return MovementWish(); };
    virtual glm::mat4x4 getOwnMatrix() { return glm::mat4x4(1.0f); };
    virtual bool checkEol() { return false; };
    FakeObject(): def(MovableObjectDefinition(2.0f, 3.0f, 4.0f)) {};
    virtual ~FakeObject() = default;
};

TEST_F(ManagedObjectInstanceTest, update_gravity_check_validity) {

    // Checks validity usage

    std::shared_ptr<FakeObject> object = make_unique<FakeObject>();

    PointOfView mainPosition;

    GravityInformation gravity_1(
        glm::vec3(0.0f, -2.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        2.0f);
    GravityInformation gravity_2{
        glm::vec3(0.0f, -3.0f, 0.0f),
        glm::vec3(0.0f, 2.0f, 0.0f),
        1.0f };

    std::unique_ptr<MockGravityProvider> gravity = std::make_unique<MockGravityProvider>();
    MockGravityProvider& ref_gravity = *gravity;

    EXPECT_CALL(ref_gravity,
        getGravityInformation(_, _ , object->def.weight,  object->def.radius, _))
        .Times(2)
        .WillOnce(Return(gravity_1))
        .WillOnce(Return(gravity_2));

    std::unique_ptr<ManagedObjectInstance> object_instance = std::make_unique<ManagedObjectInstance>(
        object,
        mainPosition,
        nullptr,
        gravity.get());

    object_instance->updateGravity(2.0f, 1.0f);

    ASSERT_FLOAT_EQ(4.0f, object_instance->gravity_validity);
    ASSERT_EQ(gravity_1.up, object_instance->current_up);
    ASSERT_EQ(gravity_1.gravity, object_instance->current_gravity);

    object_instance->updateGravity(3.0f, 1.0f);

    ASSERT_FLOAT_EQ(4.0f, object_instance->gravity_validity);
    ASSERT_EQ(gravity_1.up, object_instance->current_up);
    ASSERT_EQ(gravity_1.gravity, object_instance->current_gravity);

    object_instance->updateGravity(5.0f, 2.0f);

    ASSERT_FLOAT_EQ(6.0f, object_instance->gravity_validity);
    ASSERT_EQ(gravity_2.up, object_instance->current_up);
    ASSERT_EQ(gravity_2.gravity, object_instance->current_gravity);
}


TEST_F(ManagedObjectInstanceTest, update_gravity_check_rotation) {

    // Checks that rotation is applied around gravity vector

    std::shared_ptr<FakeObject> object = make_unique<FakeObject>();

    PointOfView mainPosition {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::mat4(1.0f),
        "room1"
    };

    GravityInformation gravity_1{
        glm::vec3(1.0f, 0.0f, 0.0f), // this is gravity
        glm::vec3(0.0f, 0.0f, 1.0f), // this is up
        10.0f };

    std::unique_ptr<MockGravityProvider> gravity = std::make_unique<MockGravityProvider>();
    MockGravityProvider& ref_gravity = *gravity;

    EXPECT_CALL(ref_gravity,
        getGravityInformation(_, _ , object->def.weight,  object->def.radius, _))
        .Times(1)
        .WillOnce(Return(gravity_1));

    std::unique_ptr<ManagedObjectInstance> object_instance = std::make_unique<ManagedObjectInstance>(
        object,
        mainPosition,
        nullptr,
        gravity.get());


    // Check that main vector is half rotated
    object_instance->updateGravity(2.0f, 0.25f); // rotate by PI/4
    auto new_up = object_instance->getObjectPosition().getUp();
    console->debug("new_up {}", to_string(new_up));
    ASSERT_TRUE( glm::length(new_up - glm::vec3(0.0, 0.707, 0.707)) < 0.01f );

    // Check that main vector is fully rotated
    object_instance->updateGravity(2.25f, 0.25f);
    new_up = object_instance->getObjectPosition().getUp();
    console->debug("new_up {}", to_string(new_up));
    ASSERT_TRUE( glm::length(new_up - glm::vec3(0.0 , 0.0 , 1.0 )) < 0.01f );

}

