#pragma once

#include "node.h"
#include "engine/path.h"

namespace game {
    class Scene {
    public:
        explicit Scene(Universe *universe, const Path &path);

        [[nodiscard]] constexpr Node *root() const { return mRoot; }
    private:
        Node *mRoot;
    };
}
