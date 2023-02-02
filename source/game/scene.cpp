#include "scene.h"
#include "engine/allocator.h"

#include <queue>
#include <array>
#include <cassert>

namespace game {
    constexpr uint32_t MaxObjects = 1000;

    class SceneImpl : public Scene {
    public:
        explicit SceneImpl(Allocator &allocator)
            : mAllocator(allocator)
        {
            initializeObjects();
        }

        void update(float dt) override {
            throw std::runtime_error("Not implemented yet.");
        }

        void render() override {
            throw std::runtime_error("Not implemented yet.");
        }

        Object createObject() override {
            if (mFreeObjects.empty()) {
                throw std::runtime_error("Max objects amount exceeded.");
            }

            auto id = mFreeObjects.front();
            mFreeObjects.pop();

            mObjectCount++;

            return Object { id };
        }

        void destroyObject(Object object) override {
            assert(object.id() <= MaxObjects);

            mFreeObjects.push(object);

            mSignatures[object.id()].reset();
            mFreeObjects.push(object);

            mObjectCount--;
        }

    private:
        Allocator &mAllocator;

        std::queue<Object> mFreeObjects;
        std::array<Signature, MaxObjects> mSignatures;
        uint32_t mObjectCount = 0;

        void initializeObjects() {
            for (uint32_t i = 0; i < MaxObjects; i++) {
                mFreeObjects.push(Object { i });
            }
        }
    };

    std::unique_ptr<Scene> Scene::createInstance(Allocator &allocator) {
        return std::make_unique<SceneImpl>(allocator);
    }
}