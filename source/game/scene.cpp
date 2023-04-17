#include "scene.h"
#include "engine/file_reader.h"

#include <json/reader.h>
#include <unordered_map>

namespace game {
    Node* readNode(Universe *universe, Json::Value &object);
    Transform readTransform(Json::Value &object);
    void readChildren(Universe *universe, Node *node, Json::Value &object);

    Scene::Scene(Universe *universe, const Path &path) {
        FileReader fileReader(path.value());

        Json::Reader reader;
        Json::Value object;

        reader.parse(fileReader.getFileContent(), object);

        mRoot = readNode(universe, object);
        auto &children = object["children"];
        for (auto &child : children) {
            mRoot->addChild(readNode(universe, child));
        }
    }

    Node* readNode(Universe *universe, Json::Value &object) {
        auto type = object["type"];
        auto transform = readTransform(object);

        auto nodeTypeMap = std::unordered_map<std::string, NodeType> {
            { "Node", NodeType::Node },
            { "Sprite", NodeType::SpriteNode },
        };

        auto nodeType = nodeTypeMap[type.asString()];
        switch (nodeType) {
            case NodeType::Node:
                return new Node(universe, transform);
            case NodeType::SpriteNode:
                return new SpriteNode(universe, transform);
        }
    }

    Transform readTransform(Json::Value &object) {
        auto &transform = object["transform"];
        auto &position = transform["position"];

        auto x = position["x"].asFloat();
        auto y = position["y"].asFloat();
        auto z = position["z"].asFloat();

        return Transform(x, y, z);
    }

    void readChildren(Universe *universe, Node *node, Json::Value &object) {
        auto &children = object["children"];
        for (auto &child : children) {
            auto childNode = readNode(universe, child);
            node->addChild(childNode);
            readChildren(universe, childNode, child);
        }
    }
}