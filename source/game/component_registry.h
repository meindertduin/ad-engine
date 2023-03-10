#pragma once

#include "component_array.h"
#include "engine/logging.h"
#include "engine/engine.h"

#include <algorithm>
#include <memory>

namespace game {
    struct IComponentArrayStaticData {
        virtual ~IComponentArrayStaticData() = default;

        ComponentType type { 0 };
        [[nodiscard]] virtual std::shared_ptr<IComponentArray> createComponentArray() const = 0;
    };

    template <typename T>
    struct ComponentArrayStaticData : public IComponentArrayStaticData {
        [[nodiscard]] std::shared_ptr<IComponentArray> createComponentArray() const override {
            return std::make_shared<ComponentArray<T>>(Engine::instance().allocator());
        }
    };

    class ComponentRegistry {
    public:
        using ComponentArrayStaticDataMap = std::unordered_map<ComponentType, std::shared_ptr<IComponentArrayStaticData>>;

        template<typename T>
        static void registerComponent() {
            if (sComponents.find(T::type()) != sComponents.end()) {
                throw std::runtime_error("Component already registered");
            }

            sComponents.insert({ T::type(), std::make_shared<ComponentArrayStaticData<T>>() });
        }

        [[nodiscard]] static const ComponentArrayStaticDataMap& componentsStaticData() {
            return sComponents;
        }

    private:
        static inline ComponentArrayStaticDataMap sComponents;
    };

    namespace detail {
        inline uint32_t sComponentCount = 0;

        template<typename T>
        struct ComponentTypeRegister {
            ComponentTypeRegister() {
                registerType();
            }

            uint32_t registerType() const {
                if (sComponentCount >= MaxComponents) {
                    throw std::runtime_error("Too many components");
                }

                T::mType = sComponentCount++;
                LOG_DEBUG("Registered {} component type with id {}", typeName<T>(), T::mType);

                ComponentRegistry::registerComponent<T>();

                return T::mType;
            }
        };
    }
}