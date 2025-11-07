#pragma once
#include "node.hpp"

template <typename T>
class DecisionTree {

public:
    //The constructors do not initialize a full tree - this is the responsibility of main.cpp
    DecisionTree() : head_(nullptr) {}
    explicit DecisionTree(Node<T>* root) : head_(root) {}

    Node<T>* getHead() const { return head_; }
    void setHead(Node<T>* root) { head_ = root; }
    void runTree(T input) { head_->runInput(input); }

private:
    Node<T>* head_;
};