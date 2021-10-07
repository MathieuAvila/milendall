#pragma once

#include <string>
#include <map>
#include <stdexcept>

#include "file_library.hxx"

class ScriptState;

class Script
{
    class ScriptState;
    std::unique_ptr<ScriptState> state;

    public:

    class ScriptException: public std::runtime_error
    {
    public:
        ScriptException(const std::string err): runtime_error(err) {};
    };

    using ValueTable = std::map<std::string, float>;

    Script(FileLibrary::UriReference file);
    virtual ~Script();

    ValueTable run(std::string functionName, ValueTable values);
};
