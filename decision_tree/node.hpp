#pragma once
#include <stdexcept>
#include <vector>
#include <map>
#include <unordered_map>
#include "../data_container/data_container.hpp"
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
    //I'm assuming features are gonna stay strings which would be convenient
    std::unordered_map<std::string, T> featuresMap;
    double impurity;
    //This bool will identify if a node needs to recalculate it's impurity. If it is frozen, the impurity is accurate
    bool frozen;
    int nSamples;
    static int& idCounter() {
        static int counter = 0;
        return counter;
    }
    static int nextId() { return idCounter()++; }
public:
    static void resetIdCounter(int start = 0) { idCounter() = start; }
    static int peekNextId() { return idCounter(); }

    Node(T value)
        : id(nextId()), classifierValue(value), leftChild(nullptr), rightChild(nullptr), featureIndex(0), impurity(0.0), frozen(true), nSamples(0) {}
    Node(T value, Node* left, Node* right)
        : id(nextId()), classifierValue(value), leftChild(left), rightChild(right), featureIndex(0), impurity(0.0), frozen(true), nSamples(0) {}
    const int getId() const { return id; }
    const bool getIsLeaf() const { return leftChild == nullptr && rightChild == nullptr; } 
    const T getClassifierValue() const { return classifierValue; }
    const Node* getLeftChild() const { return leftChild; }
    const Node* getRightChild() const { return rightChild; }
    const int getFeatureIndex() const { return featureIndex; }
    const std::unordered_map<std::string, T>& getFeatureMap() const {return featuresMap; }
    const double getImpurity() {
        return frozen ? impurity : calculateImpurityScore();
    }
    const int getNumberSamples() const {
        return nSamples;
    }
    void setLeftChild(Node* left) { leftChild = left; }
    void setRightChild(Node* right) { rightChild = right; }
    void setChildren(Node* left, Node* right) {leftChild = left; rightChild = right; }
    void setClassifierValue(T value) { classifierValue = value; }
    const void resetFeatureMap() { featuresMap.clear(); }
    void setFeatureIndex(int newIndex) {featureIndex = newIndex; }
    void resetSamples() { nSamples = 0; }
    //Increments and returns new value
    int incrementSamples() { nSamples++; return nSamples; }
    //returns the node which the container finishes on
    Node* runInput(const DataContainer& container) {
        frozen = false;
        incrementSamples();
        std::vector<T> features = container.getFeatures();
        std::string label = container.getLabel();
        this->featuresMap.emplace(label, 0);
        featuresMap[label] += 1;
        Node* currentNode = this;
        if (leftChild == nullptr || rightChild == nullptr) {
            return currentNode;
        }
        T input = features.at(featureIndex);
        if (input >= classifierValue) {
            currentNode = rightChild->runInput(container);
        } else {
            currentNode = leftChild->runInput(container);
        }
        return currentNode;
    }
    double calculateImpurityScore() {
        frozen = true;
        int totalElements = getNumberSamples();
        double currentImpurity = 1.0;
        for (auto [key, value] : featuresMap) {
            double classStdDev = value / totalElements;
            double classVariance = classStdDev * classStdDev;
            currentImpurity = currentImpurity - classVariance;
        }
        this->impurity = currentImpurity;
        return currentImpurity;
        
    }
    //Calculates impurity score of this node and all children
    double calculateImpurityForward() {
        frozen = true;
        double thisImpurity = calculateImpurityScore();
        if (leftChild != nullptr) {
            leftChild->calculateImpurityForward();
        }
        if (rightChild != nullptr) {
            rightChild->calculateImpurityForward();
        }
        return thisImpurity;
    }




};