#include "trigger.hxx"
#include "common.hxx"
#include "states_list.hxx"
#include "json_helper_accessor.hxx"
#include "json_helper_math.hxx"

/** @brief Stores the states of a Level, with an accessor.
 * Basically, this is a a map<string,bool> accessor.
 * By default statuses are false when they are not yet set
 */

static auto console = getConsole("trigger");

Trigger::Trigger()
{
}

Trigger::Trigger(nlohmann::json& json, StatesList* _events) : events(_events)
{

    if (json.contains("is_box_mode_enter")) {
        KIND = BOX_MODE;
        box_mode_values.on_enter = jsonGetElementByName(json, "is_box_mode_enter").get<bool>();
    }
    auto json_box = jsonGetElementByName(json, "box");
    borders = MinMaxBox(
        jsonGetVec3(json_box[0]),
        jsonGetVec3(json_box[1]));
    trigger_to = jsonGetElementByName(json, "target_value").get<bool>();
    event = jsonGetElementByName(json, "set_trigger").get<string>();
}

void Trigger::applyTrigger(
    const glm::vec3& previous_position,
    const glm::vec3& next_position,
    const float delta_time,
    const bool activated) const
{
    switch (KIND) {
        case BOX_MODE: {
            bool prev = borders.isIn(previous_position);
            bool next = borders.isIn(next_position);
            if (prev != next) {
                if ((box_mode_values.on_enter && next) || (!box_mode_values.on_enter && prev)){
                    events->setState(event, trigger_to);
                }
            }
            break;
        }
        case ACTIVATE: {

        }
    }
}

