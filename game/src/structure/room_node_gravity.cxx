#include "common.hxx"
#include "room_node_gravity.hxx"
#include "script.hxx"
#include "json_helper_accessor.hxx"
#include "json_helper_math.hxx"
#include "level_exception.hxx"

static auto console = getConsole("room_node_gravity");

using namespace std;

bool RoomNodeGravity::getGravityInformation(
            glm::vec3 position,
            glm::vec3 speed,
            float weight,
            float radius,
            float total_time,
            GravityInformation& gravity) const
{
    if (gravity_box.isIn(position))
    {
        switch (provider_type)
        {
            case NONE:
                return false;
            case CONST:
                gravity = constValues;
                return true;
            break;
            case SCRIPT: {
                Script::ValueTable values;
                values.insert(pair<string, float>("x", position.x));
                values.insert(pair<string, float>("y", position.y));
                values.insert(pair<string, float>("z", position.z));
                values.insert(pair<string, float>("vx", speed.x));
                values.insert(pair<string, float>("vy", speed.y));
                values.insert(pair<string, float>("vz", speed.z));
                values.insert(pair<string, float>("w", weight));
                values.insert(pair<string, float>("r", radius));
                values.insert(pair<string, float>("t", total_time));
                string script_name = "gravity_" + name;
                if (!room_script) {
                    throw LevelException("Missing script in room for: " + script_name);
                }
                try {
                    auto result = room_script->run(script_name, values);
                    gravity.gravity.x = result["g_x"];
                    gravity.gravity.y = result["g_y"];
                    gravity.gravity.z = result["g_z"];
                    gravity.up.x = result["u_x"];
                    gravity.up.y = result["u_y"];
                    gravity.up.z = result["u_z"];
                    gravity.space_kind = GravityInformation::GROUND;
                    gravity.validity = result["v"];
                }
                catch (Script::ScriptException e) {
                    throw LevelException("Invalid script " + script_name);
                }
                return true;
            }
        }
    }
    else return false;
}

RoomNodeGravity::RoomNodeGravity(string _name, Script* _room_script):
    name(_name),
    room_script(_room_script),
    provider_type(NONE)
{
}

void RoomNodeGravity::readParameters(nlohmann::json& json)
{
    // will default to ALL space, for simplicity
    gravity_box = MinMaxBox(true);
    provider_type = SCRIPT;

    console->debug("json = {}", to_string(json));

    if (json.contains("box")) {
        auto box = json["box"];
        glm::vec3 min = jsonGetVec3(jsonGetElementByName(box, "min"));
        glm::vec3 max = jsonGetVec3(jsonGetElementByName(box, "max"));
        gravity_box = MinMaxBox(min, max);
    }

    // Will set default values, otherwise run the script
    if (json.contains("default")) {
        provider_type = CONST;
        auto j_default = json["default"];
        constValues.gravity = jsonGetVec3(jsonGetElementByName(j_default, "gravity"));
        constValues.up = jsonGetVec3(jsonGetElementByName(j_default, "up"));
        constValues.space_kind = GravityInformation::GROUND; // TODO
        constValues.validity = 100000.0f; // big enough to have no trouble
    }

    // quick check on script
    if (provider_type == SCRIPT)
    {
        GravityInformation result;
        getGravityInformation(
            glm::vec3(),
            glm::vec3(),
            0.0,
            0.0,
            0.0,
            result);
    }
}

