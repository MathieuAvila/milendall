#include "common.hxx"
#include "room_animation.hxx"
#include "json_helper_accessor.hxx"
#include "gltf_instance_iface.hxx"
#include "gltf_model.hxx"
#include "states_list.hxx"

static auto console = getConsole("room_animation");

RoomAnimation::RoomAnimation(nlohmann::json& json, GltfModel* _model, GltfInstance* _instance, StatesList* _events):
    instance(_instance), model(_model), events(_events)
{
    time_false = jsonGetElementByName(json, "false").get<float>();
    time_true = jsonGetElementByName(json, "true").get<float>();
    animation = jsonGetElementByName(json, "name").get<string>();
    event = jsonGetElementByName(json, "event").get<string>();
    time_current = 0.0f;
}

void RoomAnimation::animationAdvance(float delta_time)
{
    auto state = events->getState(event);
    float target = state ? time_true:time_false;
    if (time_current != target) {
        if (time_current > target) {
            if (time_current-target > delta_time)
                time_current -= delta_time;
            else
                time_current = target;
        }
        else {
            if (target - time_current > delta_time)
                time_current += delta_time;
            else
                time_current = target;
        }
    }
    // apply animation
    console->debug("Call animation {} at {}", animation, time_current);
    model->applyAnimation(instance, animation, time_current);
}
