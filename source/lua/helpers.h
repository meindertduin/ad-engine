#pragma once

#include <string>
#include <glm/glm.hpp>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace lua {
    bool execute(lua_State* L, const std::string &script, const std::string &name, int resultsCount);

    template<typename T>
    inline bool isType(lua_State *L, int index) {
        return lua_islightuserdata(L, index) != 0;
    }

    template<>
    inline bool isType<int>(lua_State *L, int index) {
        return lua_isnumber(L, index) != 0;
    }

    template<>
    inline bool isType<float>(lua_State *L, int index) {
        return lua_isnumber(L, index) != 0;
    }

    template<>
    inline bool isType<glm::vec2>(lua_State *L, int index) {
        return lua_istable(L, index) != 0 && luaL_len(L, index) == 2;
    }

    template<>
    inline bool isType<glm::vec3>(lua_State *L, int index) {
        return lua_istable(L, index) != 0 && luaL_len(L, index) == 3;
    }

    template<>
    inline bool isType<glm::vec4>(lua_State *L, int index) {
        return lua_istable(L, index) != 0 && luaL_len(L, index) == 4;
    }

    template<>
    inline bool isType<const char*>(lua_State *L, int index) {
        return lua_isstring(L, index) != 0;
    }

    template<typename T>
    inline T convertType(lua_State *L, int index) {
        return static_cast<T>(lua_touserdata(L, index));
    }

    template<>
    inline const char* convertType<const char*>(lua_State *L, int index) {
        return lua_tostring(L, index);
    }

    template<>
    inline float convertType<float>(lua_State *L, int index) {
        return lua_tonumber(L, index);
    }

    template<>
    inline int convertType<int>(lua_State *L, int index) {
        return lua_tonumber(L, index);
    }

    template<>
    inline glm::vec2 convertType<glm::vec2>(lua_State *L, int index) {
        glm::vec2 v;

        lua_rawgeti(L, index, 1);
        v.x = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, index, 2);
        v.y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        return v;
    }

    template<>
    inline glm::vec3 convertType<glm::vec3>(lua_State *L, int index) {
        glm::vec3 v;

        lua_rawgeti(L, index, 1);
        v.x = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, index, 2);
        v.y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, index, 3);
        v.z = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        return v;
    }

    template<>
    inline glm::vec4 convertType<glm::vec4>(lua_State *L, int index) {
        glm::vec4 v;

        lua_rawgeti(L, index, 1);
        v.x = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, index, 2);
        v.y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, index, 3);
        v.z = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, index, 4);
        v.w = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        return v;
    }

    template<typename T>
    T checkArg(lua_State* L, int index) {
        if (!isType<T>(L, index)) {
            luaL_error(L, "Invalid argument type");
        }

        return convertType<T>(L, index);
    }

    void createSystemVariable(lua_State *L, const char* systemName, const char* varName, void* value);
    void createSystemVariable(lua_State *L, const char* systemName, const char* varName, int value);
    void createSystemFunction(lua_State *L, const char* systemName, const char* varName, lua_CFunction function);
    void createSystemClosure(lua_State *L, const char* systemName, void* objectPtr, const char* varName, lua_CFunction function);
}
