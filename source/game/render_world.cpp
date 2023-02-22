#include "render_world.h"
#include "ecs.h"
#include "gfx/render_component.h"
#include "transform.h"

namespace game {
    RenderWorld::RenderWorld(Scene &scene)
        : mScene(scene)
        , mRenderPipeline(gfx::RenderPipeline::createInstance(640, 480))
    {
        mRenderPipeline->initialize();
    }

    void RenderWorld::render() {
        mRenderPipeline->beforeRender();

        auto transformComponentArray = mScene.ecs().getComponentArray<Transform>();

        auto componentArray = mScene.ecs().getComponentArray<gfx::RenderComponent>();
        for (auto it = componentArray->begin(); it != componentArray->end(); it++) {
            auto &object = it.key();
            auto &component = it.value();

            auto transform = transformComponentArray->get(object);
            mRenderPipeline->renderObject(transform, component);
        }

        mRenderPipeline->afterRender();
    }
}
