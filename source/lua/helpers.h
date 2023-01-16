#pragma once

#include <string>
#include <glm/glm.hpp>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace lua {
    bool execute(lua_State* L, const std::string &script, const std::string &name, int results_count);

    template<typename T>
    inline bool is_type(lua_State *L, int index) {
        return lua_islightuserdata(L, index) != 0;
    }

    template<>
    inline bool is_type<int>(lua_State *L, int index) {
        return lua_isnumber(L, index) != 0;
    }

    template<>
    inline bool is_type<float>(lua_State *L, int index) {
        return lua_isnumber(L, index) != 0;
    }

    template<>
    inline bool is_type<glm::vec2>(lua_State *L, int index) {
        return lua_istable(L, index) != 0 && luaL_len(L, index) == 2;
    }

    template<>
    inline bool is_type<glm::vec3>(lua_State *L, int index) {
        return lua_istable(L, index) != 0 && luaL_len(L, index) == 3;
    }

    template<>
    inline bool is_type<glm::vec4>(lua_State *L, int index) {
        return lua_istable(L, index) != 0 && luaL_len(L, index) == 4;
    }

    template<>
    inline bool is_type<const char*>(lua_State *L, int index) {
        return lua_isstring(L, index) != 0;
    }

    template<typename T>
    inline T convert_type(lua_State *L, int index) {
        return static_cast<T>(lua_touserdata(L, index));
    }

    template<>
    inline const char* convert_type<const char*>(lua_State *L, int index) {
        return lua_tostring(L, index);
    }

    template<>
    inline float convert_type<float>(lua_State *L, int index) {
        return lua_tonumber(L, index);
    }

    template<>
    inline int convert_type<int>(lua_State *L, int index) {
        return lua_tonumber(L, index);
    }

    template<>
    inline glm::vec2 convert_type<glm::vec2>(lua_State *L, int index) {
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
    inline glm::vec3 convert_type<glm::vec3>(lua_State *L, int index) {
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
    inline glm::vec4 convert_type<glm::vec4>(lua_State *L, int index) {
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
    T check_arg(lua_State* L, int index) {
        if (!is_type<T>(L, index)) {
            luaL_error(L, "Invalid argument type");
        }

        return convert_type<T>(L, index);
    }

    void create_system_variable(lua_State *L, const char* system_name, const char* var_name, void* value);
    void create_system_variable(lua_State *L, const char* system_name, const char* var_name, int value);
    void create_system_function(lua_State *L, const char* system_name, const char* var_name, lua_CFunction function);
    void create_system_closure(lua_State *L, const char* system_name, void* object_ptr, const char* var_name, lua_CFunction function);
}
