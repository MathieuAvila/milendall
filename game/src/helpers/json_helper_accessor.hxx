#pragma once

#include <nlohmann/json.hpp>

/**
 * Helper function to return an element given the whole JSON and an index
 * Throws in case of bad index.
 */
nlohmann::json& jsonGetElementByIndex(nlohmann::json&, std::string element, int index);

/**
 * Helper function to return an element given its name. Check validity and throw if not present
 */
nlohmann::json& jsonGetElementByName(nlohmann::json&, std::string element);

/**
 * Helper function to return an element given its index. Check validity and throw if not present
 */
nlohmann::json& jsonGetIndex(nlohmann::json& file, int index);

/**
 * Helper function to execute code if node exists
 */
void jsonExecuteIfElement(
    nlohmann::json&, std::string,
    std::function<void(nlohmann::json&)>);

/**
 * Helper function to execute code for each element of node if it exists
 */
void jsonExecuteAllIfElement(
    nlohmann::json&, std::string element,
    std::function<void(nlohmann::json&, int)>);

