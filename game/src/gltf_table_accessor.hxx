#ifndef GLTF_TABLE_ACCESSOR_HXX
#define GLTF_TABLE_ACCESSOR_HXX

#include <nlohmann/json.hpp>

/**
 * Helper function to return an element given the whole JSON and an index
 * Throws in case of bad index.
 */
nlohmann::json& gltfGetElementByIndex(nlohmann::json&, std::string element, int index);

/**
 * Helper function to return an element given its name. Check validity and throw if not present
 */
nlohmann::json& gltfGetElementByName(nlohmann::json&, std::string element);

#endif
