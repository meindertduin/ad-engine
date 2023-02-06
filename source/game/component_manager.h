#pragma once

#include "object.h"
#include "component_registry.h"
#include "component_array.h"

#include <stdexcept>
#include <optional>
#include <memory>

namespace game {
    class ComponentManager {
    public:
        explicit ComponentManager(const ComponentRegistry &registry) {
            for (const auto &[componentType, staticData] : registry.componentsStaticData()) {
                mComponentArrays.insert({ componentType, staticData->createComponentArray() });
            }
        }

        ComponentManager(const ComponentManager &) = delete;
        ComponentManager &operator=(const ComponentManager &other) = delete;

        template<typename T>
        void addComponent(Object object, T component) {
            getComponentArray<T>()->insert(object, component);
        }

        template<typename T>
        void removeComponent(Object object) {
            getComponentArray<T>()->remove(object);
        }

        template<typename T>
        T &getComponent(Object object) {
            return getComponentArray<T>()->get(object);
        }

        template<typename T>
        T* getComponentPtr(Object object) {
            return getComponentArray<T>()->getPtr(object);
        }

        template<typename T>
        std::optional<T*> tryGetComponentPtr(Object object) {
            return getComponentArray<T>()->tryGetPtr(object);
        }

        void entityDestroyed(Object object) {
            for (auto const& [type, componentArray] : mComponentArrays) {
                componentArray->entityDestroyed(object);
            }
        }

        template<typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray() {
            if (mComponentArrays.find(T::type()) == mComponentArrays.end()) {
                throw std::runtime_error("Component wasn't registered");
            }

            return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[T::type()]);
        }
    private:
        std::unordered_map<ComponentType , std::shared_ptr<IComponentArray>> mComponentArrays;
    };
}