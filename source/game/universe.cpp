#include "universe.h"
#include "render_world.h"

#include <cassert>

#include "ecs.h"
#include "engine/application.h"
#include "scene.h"

namespace game {
    class UniverseImpl : public Universe {
    public:
        UniverseImpl()
            : mScene(this, Path { "assets/scene/demo.json" })
        {
        }

        void initialize() override {
            auto frameDimensions = math::Size2D { 640, 480 };

            mRenderWorld = std::make_unique<RenderWorld>(*this, frameDimensions);
            mRenderWorld->resize(frameDimensions);
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

        Scene* scene() override {
            return &mScene;
        }
    private:
        Ecs mEcs;
        std::unique_ptr<RenderWorld> mRenderWorld;

        Scene mScene;

        std::shared_ptr<Observer<WindowEvent>> mWindowEventObserver { nullptr };
    };

    std::unique_ptr<Universe> Universe::createInstance() {
        return std::make_unique<UniverseImpl>();
    }
}