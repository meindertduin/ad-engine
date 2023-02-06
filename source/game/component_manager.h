#pragma once

#include "object.h"
#include <map>
#include <stdexcept>
#include <optional>
#include <memory>

namespace game {
    class ComponentManager {
    public:
        class IComponentArray {
        public:
            virtual ~IComponentArray() = default;
            virtual void entityDestroyed(Object object) = 0;
        };

        template <typename T>
        class ComponentArray : public IComponentArray {
        public:
            void insert(Object object, T component) {
                if (mComponents.find(object) != mComponents.end()) {
                    throw std::runtime_error("Component already exists");
                }

                mComponents.insert({ object, component });

                mSize++;
            }

            void remove(Object object) {
                if (mComponents.find(object) == mComponents.end()) {
                    throw std::runtime_error("Component does not exist");
                }

                mComponents.erase(object);

                mSize--;
            }

            T &get(Object object) {
                if (mComponents.find(object) == mComponents.end()) {
                    throw std::runtime_error("Component does not exist");
                }

                return mComponents[object];
            }

            T* getPtr(Object object) {
                if (mComponents.find(object) == mComponents.end()) {
                    return nullptr;
                }

                return &mComponents[object];
            }

            std::optional<T*> tryGetPtr(Object object) {
                if (mComponents.find(object) == mComponents.end()) {
                    return std::nullopt;
                }

                return &mComponents[object];
            }

            auto begin() { return mComponents.begin(); }
            auto end() { return mComponents.end(); }
        private:
            std::map<Object, T> mComponents { 128 };
            size_t mSize { 0 };
        };


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
        std::map<uint32_t, std::shared_ptr<IComponentArray>> mComponentArrays;
    };
}