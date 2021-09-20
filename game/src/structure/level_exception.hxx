#pragma once

#include<stdexcept>

/** @brief Any exception related to loader */
class LevelException: public std::runtime_error
{
    public:

        LevelException(const std::string err): runtime_error(err) {};
};
