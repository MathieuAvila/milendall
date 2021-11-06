#pragma once

#include <memory>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <glm/vec3.hpp>

#include "min_max_box.hxx"

#include <gtest/gtest_prod.h>

class StatesList;

class Trigger
{
protected:

    enum { BOX_MODE, ACTIVATE } KIND;

    std::string event;
    MinMaxBox borders;


    struct {
        bool on_enter;
    } box_mode_values;

    bool trigger_to;

    StatesList* events;

public:

    /** @brief Default initialization. Won't do anything. */
    Trigger();

    /** @brief Initialization from a JSON string
     * @param json the JSON element "extras/trigger"
     * @param events The events source list that this rtigger sets its events to.
     * It is expected to be valid for the lifespan of the object.
     */
    Trigger(nlohmann::json& json, StatesList* _events);

    /** @brief Check and applies the trigger.
     * @param previous_position Previous player's position
     * @param next_position Current player's position
     * @param delta_time Elapsed time
     * @param activated If the user has hit the activate key
     */
    void applyTrigger(
        const glm::vec3 & previous_position,
        const glm::vec3 & next_position,
        const float delta_time,
        const bool activated) const;

    FRIEND_TEST(TriggerTest, triggers);
};
