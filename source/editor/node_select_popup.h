#pragma once

#include <functional>
#include "game/node.h"

namespace editor {

    class NodeSelectPopup {
    public:
        explicit NodeSelectPopup(const char *id, const std::function<void(game::NodeType, const std::string &name)> &onNodeSelect);

        bool update();
    private:
        const char *mId { nullptr };
        std::function<void(game::NodeType, const std::string &name)> mOnNodeSelect;
    };

}