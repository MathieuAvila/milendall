#include "i18n_string.hxx"
#include "json_helper_accessor.hxx"
#include "common.hxx"

static auto console = getConsole("i18n_string");

I18NString::I18NString(nlohmann::json& text, std::string name)
{
    // ATM load only the strings from the JSON
    if (!text.contains(name)) return;
    for(auto room_it : jsonGetElementByName(text, name)) {
            auto lang_code = jsonGetElementByName(room_it, "lang").get<std::string>();
            auto value = jsonGetElementByName(room_it, "value").get<std::string>();
            console->debug("Found lang={} value={}", lang_code, value);
            values.insert({lang_code, value});
    }
}

std::string I18NString::getString(const std::string locale, const std::string default_locale) const
{
    if (values.count(locale))
        return values.at(locale);
    if (values.count(default_locale))
        return values.at(default_locale);
    if (values.size())
        return values.rbegin()->second;
    return "";
}
