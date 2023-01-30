#include "shader_manager.h"
#include "lua/helpers.h"
#include "engine/file_reader.h"
#include "engine/engine.h"

namespace gfx {
    namespace lua_api {
        static Shader* get_shader(lua_State *L) {
            lua_getglobal(L, "this");
            auto shader = lua::convertType<Shader*>(L, -1);
            lua_pop(L, 1);
            return shader;
        }

        static int addStage(lua_State *L) {
            auto shader = get_shader(L);
            auto shaderType = std::string { lua::checkArg<const char*>(L, 1) };
            auto path = std::string { lua::checkArg<const char*>(L, 2) };

            ShaderStage newStage {Engine::instance().allocator() };

            static std::unordered_map<std::string, ShaderType> shaderTypeMap {
                { "Vertex", ShaderType::Vertex },
                { "Fragment", ShaderType::Fragment }
            };

            auto shaderTypeIt = shaderTypeMap.find(shaderType);
            if (shaderTypeIt == shaderTypeMap.end()) {
                throw std::runtime_error("Invalid shader type");
            }

            newStage.type = shaderTypeIt->second;

            FileReader fileReader { path };
            auto fileContent = fileReader.getFileContent();
            newStage.data.reserve(fileContent.size());

            // TODO - Implement a back_inserter for new Vector type and read from stream in fileReader
            for (auto &c : fileContent) {
                newStage.data.push(c);
            }

            newStage.path = Path {path };

            shader->addStage(std::move(newStage));

            return 0;
        }

        static lua_State* get_shader_state() {
            static lua_State* L { nullptr };

            if (L == nullptr) {
                L = luaL_newstate();
                luaL_openlibs(L);

                lua_pushcfunction(L, lua_api::addStage);
                lua_setglobal(L, "addStage");
            }

            return L;
        }
    }

    Shader* ShaderManager::createShader(const Path &path) {
        auto shader = std::make_unique<Shader>(Engine::instance().allocator());

        FileReader fileReader { path.value() };
        auto fileContent = fileReader.getFileContent();

        auto root_state = lua_api::get_shader_state();
        auto L = lua_newthread(root_state);
        const auto state_ref = luaL_ref(root_state, LUA_REGISTRYINDEX);

        lua_pushlightuserdata(L, shader.get());
        lua_setglobal(L, "this");

        lua::execute(lua_api::get_shader_state(), fileContent, path.value(), 0);

        luaL_unref(root_state, LUA_REGISTRYINDEX, state_ref);

        mShaders.insert({ path, std::move(shader) });

        return mShaders[path].get();
    }

    Shader *ShaderManager::getShader(const Path &path) {
        return mShaders[path].get();
    }
}