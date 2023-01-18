#include "helpers.h"


namespace lua {
    bool execute(lua_State* L, const std::string &script, const std::string &name, int resultsCount) {
        if(luaL_loadbuffer(L, script.c_str(), script.length(), name.c_str()) != 0) {
            printf("Lua failed to load script named: %s", name.c_str());
            lua_pop(L, 1);
            return false;
        }

        if (lua_pcall(L, 0, resultsCount, -2) != 0) {
            auto functionName = lua_tostring(L, -1);
            auto str = std::string { functionName };
            printf("Lua script named: %s had an error: '%s'", name.c_str(), str.c_str());
            lua_pop(L, 1);
            return false;
        }

        lua_pop(L, 0);
        return true;
    }

    void createSystemVariable(lua_State *L, const char* systemName, const char* varName, void* value) {
        lua_getglobal(L, systemName);
        if (lua_type(L, -1) == LUA_TNIL) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setglobal(L, systemName);
            lua_getglobal(L, systemName);
        }
        lua_pushlightuserdata(L, value);
        lua_setfield(L, -2, varName);
        lua_pop(L, 1);
    }

    void createSystemVariable(lua_State *L, const char* systemName, const char* varName, int value) {
        lua_getglobal(L, systemName);
        if (lua_type(L, -1) == LUA_TNIL) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setglobal(L, systemName);
            lua_getglobal(L, systemName);
        }
        lua_pushinteger(L, value);
        lua_setfield(L, -2, varName);
        lua_pop(L, 1);
    }

    void createSystemFunction(lua_State *L, const char* systemName, const char* varName, lua_CFunction function) {
        lua_getglobal(L, systemName);
        if (lua_type(L, -1) == LUA_TNIL) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setglobal(L, systemName);
            lua_getglobal(L, systemName);
        }
        lua_pushcfunction(L, function);
        lua_setfield(L, -2, varName);
        lua_pop(L, 1);
    }

    void createSystemClosure(lua_State *L, const char* systemName, void* objectPtr, const char* varName, lua_CFunction function) {
        lua_getglobal(L, systemName);
        if (lua_type(L, -1) == LUA_TNIL) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setglobal(L, systemName);
            lua_getglobal(L, systemName);
        }
        lua_pushlightuserdata(L, objectPtr);
        lua_pushcclosure(L, function, 1);
        lua_setfield(L, -2, varName);
        lua_pop(L, 1);
    }
}