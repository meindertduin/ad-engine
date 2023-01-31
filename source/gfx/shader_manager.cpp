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

            static std::unordered_map<std::string, bgfx::UniformType::Enum> uniformTypeMap{
                    {"sampler", bgfx::UniformType::Sampler},
                    {"vec4",    bgfx::UniformType::Vec4},
                    {"mat3",    bgfx::UniformType::Mat3},
                    {"mat4",    bgfx::UniformType::Mat4}
            };

            auto uniformTypeIt = uniformTypeMap.find(uniformType);
            if (uniformTypeIt == uniformTypeMap.end()) {
                throw std::runtime_error("Invalid uniform type");
            }

            uniform.type = uniformTypeIt->second;

            if (lua_gettop(L) > 2) {
                switch (lua_type(L, 3)) {
                    case LUA_TTABLE: {
                        auto len = luaL_len(L, 3);
                        switch (len) {
                            case 4:
                                uniform.value.v4 = lua::checkArg<glm::vec4>(L, 3);
                                break;
                            default:
                                luaL_error(L, "Uniform %s is not supported", uniformName.c_str());
                                break;
                        }
                        break;
                    }
                }
            }

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

    Shader* ShaderManager::createShader(const Path &path) {
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

        mShaders.insert({ path, std::move(shader) });

        return mShaders[path].get();
    }

    Shader *ShaderManager::getShader(const Path &path) {
        return mShaders[path].get();
    }
}