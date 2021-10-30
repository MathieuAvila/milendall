#pragma once

#include "states_list.hxx"
#include "common.hxx"

/** @brief Stores the states of a Level, with an accessor.
 * Basically,, this is a a map<string,bool> accessor.
 * By default statuses are false when they are not yet set
 */

static auto console = getConsole("states_list");

StatesList::StatesList()
{
}

bool StatesList::getState(std::string name)
{
    if (states.count(name))
        return states[name];
    return false;
}

void StatesList::setState(std::string name, bool value)
{
    console->debug("Setting state {} to {}", name, value);
    if (states.count(name))
        states[name] = value;
    states.insert(std::pair<std::string, bool>(name, value));
}

