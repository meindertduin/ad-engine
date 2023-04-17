#include "render_world.h"
#include "ecs.h"
#include "transform.h"
#include "gfx/material_manager.h"
#include "gfx/mesh_manager.h"

namespace game {
    RenderWorld::RenderWorld(Universe &scene, Allocator &allocator, const math::Size2D &frameDimensions)
        : mScene(scene)
        , mRenderPipeline(gfx::RenderPipeline::createInstance(allocator, frameDimensions))
    {
        mRenderPipeline->initialize();
        mTerrain.initialize();
    }

    void RenderWorld::render() {
        // Render terrain
        renderTerrain();

        // Render objects
        auto transformComponentArray = mScene.ecs().getComponentArray<Transform>();

        auto componentArray = mScene.ecs().getComponentArray<gfx::RenderComponent>();
        for (auto it = componentArray->begin(); it != componentArray->end(); it++) {
            const auto &object = it.key();
            auto &component = it.value();

            auto &transform = transformComponentArray->get(object);
            gfx::RenderCommand command { component.material().get(), transform, component.mesh().get() };

            mRenderPipeline->renderCommand(command);
        }

        mRenderPipeline->renderFrame();
    }

    void RenderWorld::renderTerrain() {
        auto terrainTransform = Transform(-50, 0, -80);

        auto &terrainData = mTerrain.terrainData();
        for (int i = 0; i < terrainData->size.width(); i++) {
            for (int j = 0; j < terrainData->size.height(); j++) {
                auto tileId = terrainData->tiles[i + j * terrainData->size.width()];
                auto &tile = terrainData->tilesSet[tileId];

                auto tileTransform = Transform(i * terrainData->tileSize * 1.05f + terrainTransform.position().x, 0, j * terrainData->tileSize * 1.05f + terrainTransform.position().z);

                auto material = tile->material();
                auto &mesh = tile->mesh();

                gfx::RenderCommand command { material.get(), tileTransform, mesh.get() };
                mRenderPipeline->renderCommand(command);
            }
        }
    }

    void RenderWorld::resize(const math::Size2D &frameDimensions) {
        mRenderPipeline->resize(frameDimensions);
    }
}
