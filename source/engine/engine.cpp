#include "engine.h"

#include "gfx/shader_manager.h"
#include "gfx/texture_manager.h"
#include "gfx/material_manager.h"

void Engine::shutdown() {
    gfx::ShaderManager::instance().cleanup();
    gfx::TextureManager::instance().cleanup();
    gfx::MaterialManager::instance().cleanup();
}
