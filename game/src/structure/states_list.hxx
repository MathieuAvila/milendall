#pragma once

#include <string>
#include <map>

/** @brief Stores the states of a Level, with an accessor.
 * Basically,, this is a a map<string,bool> accessor.
 * By default statuses are false when they are not yet set
 */
class StatesList
{
    std::map<std::string, bool> states;

    public:

    StatesList();
    bool getState(std::string);
    void setState(std::string, bool);

};
