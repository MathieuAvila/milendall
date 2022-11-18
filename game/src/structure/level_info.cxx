#include "level_info.hxx"

#include "json_helper_accessor.hxx"
#include "common.hxx"

static auto console = getConsole("level_info");

LevelInfo::LevelInfo(FileLibrary::UriReference _ref) : ref(_ref)
{
    json j_level = json::parse(ref.readStringContent());
    auto declarations = jsonGetElementByName(j_level, "declarations");
    auto game_type = jsonGetElementByName(declarations, "game_type");
    auto single_mode = jsonGetElementByName(game_type, "single_mode");
    recommended_time = jsonGetElementByName(single_mode, "execution_time").get<float>();
    name = I18NString(declarations, "name");
    section = jsonGetElementByName(declarations, "section").get<string>();
}
