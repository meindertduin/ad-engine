#include "engine.h"
#include "gfx/shader_manager.h"

void Engine::shutdown() {
    gfx::ShaderManager::instance().cleanup();
}
