#include "scene.h"
#include "render_world.h"

#include <cassert>

#include "ecs.h"
#include "engine/application.h"
#include "node.h"

namespace game {
    class SceneImpl : public Scene {
    public:
        SceneImpl(Allocator &allocator)
            : mAllocator(allocator)
        {
            mRootNode = new Node(this);
        }

        void initialize() override {
            mRenderWorld = std::make_unique<RenderWorld>(*this, mAllocator, Application::instance()->window().size());

            auto windowEventCallback = [this](const WindowEvent &value) {
                if (value.type == WindowEvent::Type::Resize) {
                    mRenderWorld->resize(value.size);
                }
            };

            mWindowEventObserver = Application::instance()->window()
                    .windowEvent()
                    .subscribe(windowEventCallback);
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
        Allocator &mAllocator;
        Ecs mEcs;
        std::unique_ptr<RenderWorld> mRenderWorld;

        Node *mRootNode;

        std::shared_ptr<Observer<WindowEvent>> mWindowEventObserver { nullptr };
    };

    std::unique_ptr<Scene> Scene::createInstance(Allocator &allocator) {
        return std::make_unique<SceneImpl>(allocator);
    }
}