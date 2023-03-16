#include "render_world.h"
#include "ecs.h"
#include "transform.h"
#include "gfx/material_manager.h"

namespace game {
    gfx::Vertex vertices[] = {
        { glm::vec3(1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) }, // 0
        { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // 1
        { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) }, // 3
        { glm::vec3(1.0f, -1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // 1
        { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // 2
        { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) }, // 3
    };

    RenderWorld::RenderWorld(Scene &scene, Allocator &allocator, const math::Size2D &frameDimensions)
        : mScene(scene)
        , mRenderPipeline(gfx::RenderPipeline::createInstance(allocator, frameDimensions))
    {
        mRenderPipeline->initialize();

        mMesh = std::make_unique<gfx::Mesh>(vertices, sizeof(vertices));
        mTerrain.initialize();
    }

    void RenderWorld::render() {
        // Render terrain
        auto terrainTransform = Transform(-20, 0, -180);
        gfx::RenderCommand terrainRenderCommand { mTerrain.material().get(), &terrainTransform, mTerrain.mesh().get() };
        mRenderPipeline->renderCommand(terrainRenderCommand);

        // Render objects
        auto transformComponentArray = mScene.ecs().getComponentArray<Transform>();

        auto componentArray = mScene.ecs().getComponentArray<gfx::RenderComponent>();
        for (auto it = componentArray->begin(); it != componentArray->end(); it++) {
            const auto &object = it.key();
            auto &component = it.value();

            auto &transform = transformComponentArray->get(object);
            gfx::RenderCommand command { component.material().get(), &transform, mMesh.get() };

            mRenderPipeline->renderCommand(command);
        }

        mRenderPipeline->renderFrame();
    }

    void RenderWorld::resize(const math::Size2D &frameDimensions) {
        mRenderPipeline->resize(frameDimensions);
    }
}
