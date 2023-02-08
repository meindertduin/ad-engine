#include "shader_manager.h"
#include "lua/helpers.h"
#include "engine/file_reader.h"
#include "engine/engine.h"

namespace gfx {
    namespace luaApi {
        static Shader* getShader(lua_State *L) {
            lua_getglobal(L, "this");
            auto shader = lua::convertType<Shader*>(L, -1);
            lua_pop(L, 1);
            return shader;
        }

        static int addStage(lua_State *L) {
            auto shader = getShader(L);
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

        static int addUniform(lua_State *L) {
            auto shader = getShader(L);

            auto uniformName = std::string{lua::checkArg<const char *>(L, 1)};
            auto uniformType = std::string{lua::checkArg<const char *>(L, 2)};

            Uniform uniform;
            uniform.name = uniformName;

            static std::unordered_map<std::string, Uniform::Type> uniformTypeMap{
                { "Float", Uniform::Type::Float },
                { "Vec2", Uniform::Type::Vec2 },
                { "Vec3", Uniform::Type::Vec3 },
                { "Vec4", Uniform::Type::Vec4 },
                { "Mat3", Uniform::Type::Mat3 },
                { "Mat4", Uniform::Type::Mat4 }
            };

            auto uniformTypeIt = uniformTypeMap.find(uniformType);
            if (uniformTypeIt == uniformTypeMap.end()) {
                throw std::runtime_error("Invalid uniform type");
            }

            uniform.type = uniformTypeIt->second;

            shader->addUniform(uniform);
        }

        static lua_State* getShaderState() {
            static lua_State* L { nullptr };

            if (L == nullptr) {
                L = luaL_newstate();
                luaL_openlibs(L);

                lua_pushcfunction(L, luaApi::addStage);
                lua_setglobal(L, "addStage");

                lua_pushcfunction(L, luaApi::addUniform);
                lua_setglobal(L, "addUniform");
            }

            return L;
        }
    }

    Handle<Shader> ShaderManager::createShader(const Path &path) {
        if (mShaderPathsIdsMap.contains(path)) {
            return Handle<Shader> { mShaderPathsIdsMap[path] };
        }

        auto shader = std::make_unique<Shader>(Engine::instance().allocator());

        FileReader fileReader { path.value() };
        auto fileContent = fileReader.getFileContent();

        auto root_state = luaApi::getShaderState();
        auto L = lua_newthread(root_state);
        const auto state_ref = luaL_ref(root_state, LUA_REGISTRYINDEX);

        lua_pushlightuserdata(L, shader.get());
        lua_setglobal(L, "this");

        lua::execute(luaApi::getShaderState(), fileContent, path.value(), 0);

        luaL_unref(root_state, LUA_REGISTRYINDEX, state_ref);

        mShaderPathsIdsMap.insert({ path, mNextId });
        mShaders.insert({ mNextId, std::move(shader) });

        return Handle<Shader> { mNextId++ };
    }

    Shader* ShaderManager::get(int id) {
        return mShaders[id].get();
    }
}