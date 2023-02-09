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

        for (auto &[object, component] : *mScene.ecs().getComponentArray<gfx::RenderComponent>()) {
            auto transform = transformComponentArray->get(object);
            mRenderPipeline->renderObject(transform, component);
        }

        mRenderPipeline->afterRender();
    }
}
