#include <gtest/gtest.h>

#include "trigger.hxx"
#include "common.hxx"
#include "states_list.hxx"

static auto console = getConsole("unit_trigger");

class TriggerTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("unit_trigger")->set_level(spdlog::level::debug);
     spdlog::get("trigger")->set_level(spdlog::level::debug);
     spdlog::get("states_list")->set_level(spdlog::level::debug);
  }
};

TEST_F(TriggerTest, box_mode_get_in) {

    StatesList events;
    auto json_element = json::parse(
        "{  "\
      " \"box\": [  [ -1.3, -0.1, -1.2 ],  "\
                   "[  2.8, 1.6, 1.2 ] ],  "\
      "\"set_trigger\": \"door_event\",  "\
      "\"is_box_mode_enter\": true,  "\
      "\"target_value\": true  "\
     "}"
     );
    auto trigger = Trigger(json_element, &events);

    // no trigger when moving outside
    trigger.applyTrigger(glm::vec3(-10.0, 0.0, 0.0), glm::vec3(-11.0, 0.0, 0.0), 0.0f, false);
    ASSERT_FALSE(events.getState("door_event"));

    // no trigger when moving inside
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 0.0f, false);
    ASSERT_FALSE(events.getState("door_event"));

    // no trigger when activate
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 0.0f, true);
    ASSERT_FALSE(events.getState("door_event"));

    // no trigger when going outside
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(100.0, 0.0, 0.0), 0.0f, false);
    ASSERT_FALSE(events.getState("door_event"));

    // trigger when going inside
    trigger.applyTrigger(glm::vec3(-10.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 0.0f, false);
    ASSERT_TRUE(events.getState("door_event"));

}

TEST_F(TriggerTest, box_mode_get_out) {

    StatesList events;
    auto json_element = json::parse(
        "{  "\
      " \"box\": [  [ -1.3, -0.1, -1.2 ],  "\
                   "[  2.8, 1.6, 1.2 ] ],  "\
      "\"set_trigger\": \"door_event\",  "\
      "\"is_box_mode_enter\": false,  "\
      "\"target_value\": true  "\
     "}"
     );
    auto trigger = Trigger(json_element, &events);

    // no trigger when moving outside
    trigger.applyTrigger(glm::vec3(-10.0, 0.0, 0.0), glm::vec3(-11.0, 0.0, 0.0), 0.0f, false);
    ASSERT_FALSE(events.getState("door_event"));

    // no trigger when moving inside
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 0.0f, false);
    ASSERT_FALSE(events.getState("door_event"));

    // no trigger when activate
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 0.0f, true);
    ASSERT_FALSE(events.getState("door_event"));

    // no trigger when going inside
    trigger.applyTrigger(glm::vec3(-10.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 0.0f, false);
    ASSERT_FALSE(events.getState("door_event"));

    // trigger when going outside
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(100.0, 0.0, 0.0), 0.0f, false);
    ASSERT_TRUE(events.getState("door_event"));

}

TEST_F(TriggerTest, box_mode_set_to_false) {

    StatesList events;
    auto json_element = json::parse(
        "{  "\
      " \"box\": [  [ -1.3, -0.1, -1.2 ],  "\
                   "[  2.8, 1.6, 1.2 ] ],  "\
      "\"set_trigger\": \"door_event\",  "\
      "\"is_box_mode_enter\": false,  "\
      "\"target_value\": false  "\
     "}"
     );
    auto trigger = Trigger(json_element, &events);
    events.setState("door_event", true); // start with true

    // no trigger when moving outside
    trigger.applyTrigger(glm::vec3(-10.0, 0.0, 0.0), glm::vec3(-11.0, 0.0, 0.0), 0.0f, false);
    ASSERT_TRUE(events.getState("door_event"));

    // no trigger when moving inside
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 0.0f, false);
    ASSERT_TRUE(events.getState("door_event"));

    // no trigger when activate
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 0.0f, true);
    ASSERT_TRUE(events.getState("door_event"));

    // no trigger when going inside
    trigger.applyTrigger(glm::vec3(-10.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 0.0f, false);
    ASSERT_TRUE(events.getState("door_event"));

    // trigger when going outside
    trigger.applyTrigger(glm::vec3(0.0, 0.0, 0.0), glm::vec3(100.0, 0.0, 0.0), 0.0f, false);
    ASSERT_FALSE(events.getState("door_event"));

}
