#include "scene.h"
#include "engine/allocator.h"
#include "render_world.h"

#include <cassert>

#include "ecs.h"
#include "engine/application.h"

namespace game {
    class SceneImpl : public Scene {
    public:
        explicit SceneImpl(Allocator &allocator)
            : mAllocator(allocator)
        {
        }

        void initialize() override {
            mRenderWorld = std::make_unique<RenderWorld>(*this, Application::instance()->window().size());
        }

        void update(float dt) override {
            throw std::runtime_error("Not implemented yet.");
        }

        void render() override {
            mRenderWorld->render();
        }

        Object createObject() override {
            return mEcs.createObject();
        }

        void destroyObject(Object object) override {
            assert(object.id() <= MaxObjects);

            mEcs.destroyObject(object);
        }

        Ecs& ecs() override {
            return mEcs;
        }

    private:
        Ecs mEcs;
        Allocator &mAllocator;
        std::unique_ptr<RenderWorld> mRenderWorld;
    };

    std::unique_ptr<Scene> Scene::createInstance(Allocator &allocator) {
        return std::make_unique<SceneImpl>(allocator);
    }
}