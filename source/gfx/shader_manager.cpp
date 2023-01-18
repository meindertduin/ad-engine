#include "shader_manager.h"
#include "lua/helpers.h"
#include "engine/file_reader.h"

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
            auto path = std::string { lua::checkArg<const char*>(L, 1) };

            ShaderStage new_stage;
            if (path.starts_with("f_")) {
                new_stage.type = ShaderType::Fragment;
            } else if (path.starts_with("v_")) {
                new_stage.type = ShaderType::Vertex;
            } else {
                // TODO throw error or handle case
            }

            FileReader fileReader { path };
            new_stage.data = fileReader.getFileContent();
            new_stage.path = Path { path };

            shader->addStage(std::move(new_stage));

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
        auto shader = std::make_unique<Shader>();

        mShaders.insert({ path, std::move(shader) });

        FileReader fileReader { path.value() };
        auto fileContent = fileReader.getFileContent();

        lua::execute(lua_api::get_shader_state(), fileContent, path.value(), 0);

        return mShaders[path].get();
    }

    Shader *ShaderManager::getShader(const Path &path) {
        return mShaders[path].get();
    }
}