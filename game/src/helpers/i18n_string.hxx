#pragma once

#include <string>
#include <map>
#include <nlohmann/json.hpp>

class ScriptState;

class I18NString
{

    std::map<std::string, std::string> values;

    public:

    /** @brief Load strings from Json, if it exists.
     * if root contains name, load table of strings from it.
     * if not, no string is loaded.
     */
    I18NString(nlohmann::json& root, std::string name);

    /** @brief default for auto ctor */
    I18NString() = default;

    ~I18NString() = default;

    /** @brief Get a localized string in order:
     * - with the locale
     * - with default locale
     * - the first available
     * - otherwise return the void string
     */
    std::string getString(const std::string locale, const std::string default_locale) const;
};
