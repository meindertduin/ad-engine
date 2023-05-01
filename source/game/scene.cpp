#include "scene.h"
#include "engine/file_reader.h"

#include <json/reader.h>
#include <unordered_map>

namespace game {
    Node* readNode(Universe *universe, Json::Value &object);
    Transform readTransform(Json::Value &object);

    Scene::~Scene() {
        for (const auto &[id, node] : mNodesMap) {
            delete node;
        }
    }

    Scene::Scene(Universe *universe, const Path &path)
        : mUniverse(universe)
    {
        FileReader fileReader(path.value());


        std::function<void(Node*, Json::Value&)> readChildren = [&](Node *node, Json::Value &object) {
            auto &children = object["children"];
            for (auto &child : children) {
                auto childNode = readNode(universe, child);
                node->addChild(childNode);
                addNode(childNode);
                readChildren(childNode, child);
            }
        };

        Json::Reader reader;
        Json::Value object;

        reader.parse(fileReader.getFileContent(), object);

        mRoot = readNode(universe, object);
        addNode(mRoot);
        readChildren(mRoot, object);
    }

    Node *Scene::getNodeById(uint32_t id) {
        return mNodesMap[id];
    }

    Node *Scene::getNodeByName(const std::string &name) {
        return getNodeById(mNodeIdsMap[name]);
    }

    void Scene::addNode(Node *node) {
        mNodesMap[node->id()] = node;
        mNodeIdsMap[node->name()] = node->id();
    }

    void Scene::removeNode(Node *node) {
        mNodesMap.erase(node->id());
        mNodeIdsMap.erase(node->name());

        node->parent()->removeChild(node);

        delete node;
    }

    Node* Scene::createNode(NodeType type, const std::string &name, Node *parent) {
        Node* node;
        switch (type) {
            case NodeType::Node:
                node = new Node(mUniverse, Transform());
                break;
            case NodeType::SpriteNode:
                node = new SpriteNode(mUniverse, Transform());
                break;
        }

        if (parent) {
            parent->addChild(node);
        } else if(mRoot) {
            mRoot->addChild(node);
        } else {
            mRoot = node;
        }

        node->setName(name);
        addNode(node);

        return node;
    }

    Node* readNode(Universe *universe, Json::Value &object) {
        auto type = object["type"];
        auto transform = readTransform(object);
        auto name = object["name"].asString();

        auto nodeTypeMap = std::unordered_map<std::string, NodeType> {
            { "Node", NodeType::Node },
            { "Sprite", NodeType::SpriteNode },
        };

        auto nodeType = nodeTypeMap[type.asString()];
        Node* node;
        switch (nodeType) {
            case NodeType::Node:
                node = new Node(universe, transform);
                break;
            case NodeType::SpriteNode:
                node = new SpriteNode(universe, transform);
                break;
        }

        node->setName(name);
        return node;
    }

    Transform readTransform(Json::Value &object) {
        auto &transform = object["transform"];
        auto &position = transform["position"];

        auto x = position["x"].asFloat();
        auto y = position["y"].asFloat();
        auto z = position["z"].asFloat();

        return { x, y, z };
    }

}