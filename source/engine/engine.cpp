#include "engine.h"

#include "gfx/shader_manager.h"
#include "gfx/texture_manager.h"
#include "gfx/material_manager.h"
#include "gfx/mesh_manager.h"


void Engine::initialize() {
    gfx::MeshManager::instance().initialize();
}

void Engine::shutdown() {
    gfx::ShaderManager::instance().cleanup();
    gfx::TextureManager::instance().cleanup();
    gfx::MaterialManager::instance().cleanup();
    gfx::MeshManager::instance().cleanup();
}
