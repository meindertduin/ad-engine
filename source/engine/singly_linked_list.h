#pragma once

 template<typename T>
class SinglyLinkedList {
public:
    struct Node {
        T data;
        Node* next = nullptr;
    };

    Node *head;

    inline void insert_back(Node* newNode) {
        if (head != nullptr) {
            head->next = newNode;
        } else {
            head = newNode;
        }
    }

    inline void insert(Node* newNode, Node* previousNode) {
        if (previousNode == nullptr) {
            // is the first node
            if (head != nullptr) {
                // list has more elements
                newNode->next = head;
            } else {
                newNode->next = nullptr;
            }
            head = newNode;
        } else {
            if (previousNode->next == nullptr) {
                // is the last node
                previousNode->next = newNode;
                newNode->next = nullptr;
            } else {
                // is a middle node
                newNode->next = previousNode->next;
                previousNode->next = newNode;
            }
        }
    }

    inline void remove(Node* removeNode, Node* previousNode) {
        if (previousNode == nullptr) {
            // removeNode is the first node
            if (removeNode->next == nullptr) {
                // list has only one element
                head = nullptr;
            } else {
                // list has more elements
                head = removeNode->next;
            }
        } else {
            previousNode->next = removeNode->next;
        }
    }
};