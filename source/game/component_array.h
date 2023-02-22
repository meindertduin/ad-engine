#pragma once

#include "object.h"
#include "engine/map.h"
#include <unordered_map>
#include <optional>
#include <stdexcept>

namespace game {
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void entityDestroyed(Object object) = 0;
    };

    template <typename T>
    class ComponentArray : public IComponentArray {
    public:
        explicit ComponentArray(Allocator &allocator)
            : mComponents(allocator)
        {
        }

        void insert(Object object, T component) {
            if (mComponents.find(object) != mComponents.end()) {
                throw std::runtime_error("Component already exists");
            }

            mComponents.insert(object, component);

            mSize++;
        }

        void remove(Object object) {
            if (mComponents.find(object) == mComponents.end()) {
                throw std::runtime_error("Component does not exist");
            }

            mComponents.remove(object);

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

        void entityDestroyed(Object object) override {
            remove(object);
        }

        auto begin() { return mComponents.begin(); }
        auto end() { return mComponents.end(); }
    private:
        HashMap<Object, T> mComponents;
        size_t mSize { 0 };
    };
}