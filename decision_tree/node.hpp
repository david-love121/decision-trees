#pragma once
#include <stdexcept>
#include <vector>
template <typename T>       
class Node {
private:
    int id;
    // The value which accepts, or sends to the right node when input >= value
    T classifierValue;
    Node* leftChild;
    Node* rightChild;
    //The feature which the node is responsible for
    int featureIndex;
    static int& idCounter() {
        static int counter = 0;
        return counter;
    }
    static int nextId() { return idCounter()++; }
public:
    static void resetIdCounter(int start = 0) { idCounter() = start; }
    static int peekNextId() { return idCounter(); }

    Node(T value)
        : id(nextId()), classifierValue(value), leftChild(nullptr), rightChild(nullptr), featureIndex(0) {}
    Node(T value, Node* left, Node* right)
        : id(nextId()), classifierValue(value), leftChild(left), rightChild(right), featureIndex(0) {}
    int getId() const { return id; }
    bool getIsLeaf() const { return leftChild == nullptr && rightChild == nullptr; } 
    T getClassifierValue() const { return classifierValue; }
    Node* getLeftChild() const { return leftChild; }
    Node* getRightChild() const { return rightChild; }
    int getFeatureIndex() const { return featureIndex; }
    void setLeftChild(Node* left) { leftChild = left; }
    void setRightChild(Node* right) { rightChild = right; }
    void setChildren(Node* left, Node* right) {leftChild = left; rightChild = right; }
    void setClassifierValue(T value) { classifierValue = value; }

    void setFeatureIndex(int newIndex) {featureIndex = newIndex; }
    //returns the node which the container finishes on
    const Node* runInput(const std::vector<T>& inputs) const {
        const Node* currentNode = this;
        if (leftChild == nullptr || rightChild == nullptr) {
            return currentNode;
        }
        T input = inputs.at(featureIndex);
        if (input >= classifierValue) {
            currentNode = rightChild->runInput(inputs);
        } else {
            currentNode = leftChild->runInput(inputs);
        }
        return currentNode;
    }

    


};