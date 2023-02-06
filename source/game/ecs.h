#pragma once

#include <queue>
#include <array>
#include <stdexcept>
#include <cassert>
#include "object.h"
#include "component_manager.h"

namespace game {
    class Ecs {
    public:
        Ecs() {
            initializeObjects();
        }

        Ecs(const Ecs &) = delete;
        Ecs &operator=(const Ecs &other) = delete;

        Object createObject() {
            if (mFreeObjects.empty()) {
                throw std::runtime_error("Max objects amount exceeded.");
            }

            auto id = mFreeObjects.front();
            mFreeObjects.pop();

            mObjectCount++;

            return Object { id };
        }

        void destroyObject(Object object) {
            assert(object.id() <= MaxObjects);

            mComponentManager.entityDestroyed(object);

            mFreeObjects.push(object);

            mSignatures[object.id()].reset();
            mFreeObjects.push(object);

            mObjectCount--;
        }

        template<typename T>
        void addComponent(Object object, T component) {
            mComponentManager.addComponent<T>(object, component);

            auto &signature = mSignatures[object.id()];
            signature.set(T::type(), true);
        }

        template<typename T>
        void removeComponent(Object object) {
            mComponentManager.removeComponent<T>(object);

            auto &signature = mSignatures[object.id()];
            signature.set(T::type(), false);
        }

        template<typename T>
        T &getComponent(Object object) {
            return mComponentManager.getComponent<T>(object);
        }

        template<typename T>
        T* getComponentPtr(Object object) {
            return mComponentManager.getComponentPtr<T>(object);
        }

        template<typename T>
        std::optional<T*> tryGetComponentPtr(Object object) {
            return mComponentManager.tryGetComponentPtr<T>(object);
        }

        template<typename T>
        bool hasComponent(Object object) {
            return mSignatures[object.id()].test(T::type());
        }

        template<typename T>
        std::shared_ptr<ComponentManager::ComponentArray<T>> getComponentArray() {
            return mComponentManager.getComponentArray<T>();
        }

    private:
        std::queue<Object> mFreeObjects;
        std::array<Signature, MaxObjects> mSignatures;
        uint32_t mObjectCount = 0;

        ComponentManager mComponentManager;

        void initializeObjects() {
            for (uint32_t i = 0; i < MaxObjects; i++) {
                mFreeObjects.emplace(i);
            }
        }
    };
}