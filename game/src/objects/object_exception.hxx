#pragma once

#include <stdexcept>

/** @brief A single GLTF exception when a loading errors happens */
class ObjectException: public std::runtime_error
{
    public:

        ObjectException(const std::string err): runtime_error(err) {};
};
