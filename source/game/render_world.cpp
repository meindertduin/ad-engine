#include "render_world.h"
#include "ecs.h"
#include "gfx/render_component.h"

namespace game {
    RenderWorld::RenderWorld(Scene &scene)
            : mScene(scene)
            , mRenderPipeline(gfx::RenderPipeline::createInstance(640, 480))
    {
        mRenderPipeline->initialize();
    }

    void RenderWorld::render() {
        mRenderPipeline->beforeRender();

        for (auto &[object, component] : *mScene.ecs().getComponentArray<gfx::RenderComponent>()) {
            mRenderPipeline->renderObject(component);
        }

        mRenderPipeline->afterRender();
    }
}
