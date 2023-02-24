#include "material_manager.h"
#include "material.h"
#include "engine/engine.h"
#include "engine/file_reader.h"
#include "lua/helpers.h"
#include "shader_manager.h"

namespace gfx {
    namespace luaApi {
        static Material* getMaterial(lua_State *L) {
            lua_getglobal(L, "this");
            auto material = lua::convertType<Material*>(L, -1);
            lua_pop(L, 1);
            return material;
        }

        static int setShader(lua_State *L) {
            auto material = getMaterial(L);
            auto shaderPath = lua::checkArg<const char*>(L, 1);

            auto shader = ShaderManager::instance().createShader(Path { shaderPath });

            material->setShader(shader);

            return 0;
        }

        static lua_State* getMaterialState() {
            static lua_State* L { nullptr };

            if (L == nullptr) {
                L = luaL_newstate();
                luaL_openlibs(L);

                lua_pushcfunction(L, luaApi::setShader);
                lua_setglobal(L, "setShader");
            }

            return L;
        }
    }

    MaterialHandle MaterialManager::createMaterial(const Path &path) {
        if (mMaterialPathsIdsMap.contains(path)) {
            return MaterialHandle { mMaterialPathsIdsMap[path] };
        }

        auto material = std::make_unique<Material>(Engine::instance().allocator());

        FileReader fileReader { path.value() };
        auto fileContent = fileReader.getFileContent();

        auto root_state = luaApi::getMaterialState();
        auto L = lua_newthread(root_state);
        const auto state_ref = luaL_ref(root_state, LUA_REGISTRYINDEX);

        lua_pushlightuserdata(L, material.get());
        lua_setglobal(L, "this");

        lua::execute(luaApi::getMaterialState(), fileContent, path.value(), 0);

        luaL_unref(root_state, LUA_REGISTRYINDEX, state_ref);

        mMaterialPathsIdsMap.try_emplace(path, mNextId);
        mMaterials.insert({ mNextId, std::move(material) });

        return MaterialHandle { mNextId++ };
    }

    Material* MaterialManager::get(uint32_t id) {
        return mMaterials[id].get();
    }
}