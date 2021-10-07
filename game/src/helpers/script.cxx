#include "script.hxx"
#include "common.hxx"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static auto console = getConsole("script");

class Script::ScriptState
{
        lua_State *L;

        void throwError(std::string msg);

    public:

        ScriptState(FileLibrary::UriReference file);
        Script::ValueTable run(std::string functionName, Script::ValueTable);
        ~ScriptState();
};


void Script::ScriptState::throwError(std::string msg){
	console->error("{}: {}", msg, lua_tostring(L, -1));
	throw Script::ScriptException(msg);
}

Script::ScriptState::ScriptState(FileLibrary::UriReference file)
{
    L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_loadstring(L, file.readStringContent().c_str()))
	    throwError("luaL_loadfile() failed");
    if (lua_pcall(L, 0, 0, 0))
	throwError("lua_pcall() failed");
}

Script::ScriptState::~ScriptState()
{
    lua_close(L);
}

Script::ValueTable Script::ScriptState::run(std::string functionName, Script::ValueTable values)
{
    Script::ValueTable result;

    lua_getglobal(L, functionName.c_str());

    lua_newtable(L);
    for (auto  [k, v] : values) {
        const char* ks = k.c_str();
        lua_pushstring(L, ks);
        lua_pushnumber(L, v);
        lua_settable(L, -3);
    }
    if (lua_pcall(L, 1, 1, 0))
	    throwError("lua_pcall() failed");
    lua_pushnil(L);
    while (lua_next(L, -2))
    {
        lua_Number n = lua_tonumber(L, -1);
        lua_pop(L,1);
        const char *k = lua_tostring(L, -1);
        console->debug("Result {}={}", k, n);
        result.insert(std::pair<std::string, float>(std::string(k), n));
    }
    return result;
}

Script::Script(FileLibrary::UriReference file)
{
    state = std::make_unique<ScriptState>(file);
}

Script::~Script()
{
}

Script::ValueTable Script::run(std::string functionName, ValueTable values)
{
    return state->run(functionName, values);
}