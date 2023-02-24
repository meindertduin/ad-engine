#include "render_world.h"
#include "ecs.h"
#include "gfx/render_component.h"
#include "transform.h"

namespace game {
    RenderWorld::RenderWorld(Scene &scene, const math::Size2D &frameDimensions)
        : mScene(scene)
        , mRenderPipeline(gfx::RenderPipeline::createInstance(frameDimensions))
    {
        mRenderPipeline->initialize();
    }

    void RenderWorld::render() {
        mRenderPipeline->beforeRender();

        auto transformComponentArray = mScene.ecs().getComponentArray<Transform>();

        auto componentArray = mScene.ecs().getComponentArray<gfx::RenderComponent>();
        for (auto it = componentArray->begin(); it != componentArray->end(); it++) {
            const auto &object = it.key();
            auto &component = it.value();

            auto transform = transformComponentArray->get(object);
            mRenderPipeline->renderObject(transform, component);
        }

        mRenderPipeline->afterRender();
    }

    void RenderWorld::resize(const math::Size2D &frameDimensions) {
        mRenderPipeline->resize(frameDimensions);
    }
}
