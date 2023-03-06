#include "render_world.h"
#include "ecs.h"
#include "transform.h"
#include "gfx/material_manager.h"

namespace game {
    RenderWorld::RenderWorld(Scene &scene, Allocator &allocator, const math::Size2D &frameDimensions)
        : mScene(scene)
        , mRenderPipeline(gfx::RenderPipeline::createInstance(allocator, frameDimensions))
    {
        mRenderPipeline->initialize();
    }

    void RenderWorld::render() {
        auto transformComponentArray = mScene.ecs().getComponentArray<Transform>();

        auto componentArray = mScene.ecs().getComponentArray<gfx::RenderComponent>();
        for (auto it = componentArray->begin(); it != componentArray->end(); it++) {
            const auto &object = it.key();
            auto &component = it.value();

            auto &transform = transformComponentArray->get(object);
            gfx::RenderCommand command { component.material().get(), &transform };

            mRenderPipeline->renderCommand(command);
        }

        mRenderPipeline->renderFrame();
    }

    void RenderWorld::resize(const math::Size2D &frameDimensions) {
        mRenderPipeline->resize(frameDimensions);
    }
}
