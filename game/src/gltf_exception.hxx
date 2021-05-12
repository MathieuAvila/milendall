#pragma once

#include <stdexcept>

/** @brief A single GLTF exception when a loading errors happens */
class GltfException: public std::runtime_error
{
    public:

        GltfException(const std::string err): runtime_error(err) {};
};
