#pragma once

#include <memory>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include <gtest/gtest_prod.h>

class GltfInstance;
class StatesList;
class GltfModel;

class RoomAnimation
{
protected:
    std::string animation;
    std::string event;
    float time_false, time_true, time_current;

    GltfInstance* instance;
    GltfModel* model;
    StatesList* events;

public:

    /** @brief Default initialization. Won't do anything. */
    RoomAnimation();

    /** @brief Initialization from a JSON string
     * @param json the JSON element "extras/animations"
     * @param model Which model holds the animations
     * @param instance Which instance to apply it to. IT is expected to be valid
     * for the lifespan of the object.
     * @param events The events source list that this animation takes its events from.
     * IT is expected to be valid for the lifespan of the object.
     */
    RoomAnimation(nlohmann::json& json, GltfModel* model, GltfInstance* instance, StatesList* events);

    /** @brief Advance and applies animation from a given time delta
     * @param delta_time delta between last call
     */
    void animationAdvance(float delta_time);

    FRIEND_TEST(RoomTest, animations);
};
