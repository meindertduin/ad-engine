#pragma once

#include <memory>
#include "object.h"
#include "engine/allocator.h"
#include "engine/format.h"

namespace game {
    class Ecs;

    class Universe {
    public:
        virtual ~Universe() = default;

        static std::unique_ptr<Universe> createInstance(Allocator &allocator);

        virtual void initialize() = 0;
        virtual void update(float dt) = 0;
        virtual void render() = 0;

        virtual Object createObject() = 0;
        virtual void destroyObject(Object object) = 0;

        virtual Ecs& ecs() = 0;
    };
}
