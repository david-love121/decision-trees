#pragma once
#include "node.hpp"
#include <map>
template <typename T>
class DecisionTree {

public:
    //The constructors do not initialize a full tree - this is the responsibility of main.cpp
    DecisionTree() : head_(nullptr) {}
    explicit DecisionTree(Node<T>* root) : head_(root) {}

    Node<T>* getHead() const { return head_; }
    void setHead(Node<T>* root) { head_ = root; nodeMap_[root->getId()] = root; }
    void runTree(T input) { head_->runInput(input); }
    void addNodes(Node<T>* newNodeL, Node<T>* newNodeR, int prevId) {nodeMap_[prevId]->setChildren(newNodeL, newNodeR); }
    void calculateAllImpurity() {
        head_->calculateImpurityForward();
    }


private:
    Node<T>* head_;
    std::map<int, Node<T>*> nodeMap_;
    

};