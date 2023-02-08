#pragma once

#include <memory>
#include "engine/allocator.h"
#include "object.h"

namespace game {
    class Ecs;

    class Scene {
    public:
        virtual ~Scene() = default;

        static std::unique_ptr<Scene> createInstance(Allocator &allocator);

        virtual void update(float dt) = 0;
        virtual void render() = 0;

        virtual Object createObject() = 0;
        virtual void destroyObject(Object object) = 0;

        virtual Ecs& ecs() = 0;
    };
}
