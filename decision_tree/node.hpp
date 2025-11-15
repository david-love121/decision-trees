#pragma once
#include <stdexcept>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>
#include "../data_container/data_container.hpp"
template <typename T>       
class Node {
private:
    int id_;
    // The value which accepts, or sends to the right node when input >= value
    T classifierValue_;
    std::unique_ptr<Node> leftChild_;
    std::unique_ptr<Node> rightChild_;
    //The feature which the node is responsible for
    int featureIndex_;
    
    double impurity_;
    //This bool will identify if a node needs to recalculate it's impurity. If it is frozen, the impurity is accurate
    bool frozen_;
    int nSamples_;
    //Holds indices of dataContainers it's seen
    std::vector<std::size_t> sampleIndices_;
    std::unordered_map<std::string, int> classCounts_;
    //Holds a cumulation of the features it has seen for average when training
    T runningValue_;
    static int& idCounter() {
        static int counter = 0;
        return counter;
    }
    static int nextId() { return idCounter()++; }
public:
    static void resetIdCounter(int start = 0) { idCounter() = start; }
    static int peekNextId() { return idCounter(); }

    explicit Node(T value = T{})
        : id_(nextId()), classifierValue_(value), leftChild_(nullptr), rightChild_(nullptr), featureIndex_(0), impurity_(0.0), frozen_(true), nSamples_(0), runningValue_(T{}) {

        }

    const int getId() const { return id_; }
    const bool getIsLeaf() const { return leftChild_ == nullptr && rightChild_ == nullptr; } 
    const T getClassifierValue() const { return classifierValue_; }
    const std::unique_ptr<Node> getLeftChild() const { return leftChild_; }
    const std::unique_ptr<Node> getRightChild() const { return rightChild_; }
    const int getFeatureIndex() const { return featureIndex_; }
    const double getImpurity() {
        return frozen_ ? impurity_ : calculateImpurityScore();
    }
    const int getNumberSamples() const {
        return nSamples_;
    }
    void setChildren(std::unique_ptr<Node> left, std::unique_ptr<Node> right) {leftChild_ = std::move(left); rightChild_ = std::move(right); }
    void setClassifierValue(T value) { classifierValue_ = value; }
    void setFeatureIndex(int newIndex) {featureIndex_ = newIndex; }
    void resetSamples() { nSamples_ = 0; }
    void resetCurrentContainers() { sampleIndices_.clear(); }
    void resetRunningValue() { runningValue_ = T{}; }
    void incrementRunningValue(T delta) {runningValue_ = runningValue_ + delta; }
    //Increments and returns new value
    int incrementSamples() { nSamples_++; return nSamples_; }
    T getAverageValue() { runningValue_ / nSamples_; }
    //returns the node which the container finishes on
    const int runInput(const DataContainer& container) {
        frozen_ = false;
        incrementSamples();
        std::vector<T> features = container.getFeatures();
        std::string label = container.getLabel();
        int currentNodeId = this->id_;

        if (this->getIsLeaf()) {
            sampleIndices_.push_back(container.getId());
            classCounts_.emplace(container.getLabel(), 0);
            classCounts_[container.getLabel()]++;
            incrementRunningValue(features.at(featureIndex_));
            return currentNodeId;
        }

        T input = features.at(featureIndex_);

        if (input >= classifierValue_) {
            currentNodeId = rightChild_->runInput(container);
        } else {
            currentNodeId = leftChild_->runInput(container);
        }
        return currentNodeId;
    }
    double calculateImpurityScore() {
        frozen_ = true;
        int totalElements = getNumberSamples();
        double currentImpurity = 1.0;
        for (auto [key, value] : classCounts_) {
            double classStdDev = (double)value / totalElements;
            double classVariance = classStdDev * classStdDev;
            currentImpurity = currentImpurity - classVariance;
        }
        this->impurity_ = currentImpurity;
        return currentImpurity;
        
    }
    //Calculates impurity score of this node and all children
    double calculateImpurityForward() {
        frozen_ = true;
        double thisImpurity = calculateImpurityScore();
        if (leftChild_ != nullptr) {
            leftChild_->calculateImpurityForward();
        }
        if (rightChild_ != nullptr) {
            rightChild_->calculateImpurityForward();
        }
        return thisImpurity;
    }
    void resetNode() {
        this->resetCurrentContainers();
        this->resetRunningValue();
        this->resetSamples();
        this->frozen_ = false;
    }
    void resetNodeRecursive() {
        this->resetNode();
        this->leftChild_->resetNodeRecursive();
        this->rightChild_->resetNodeRecursive();
    }

    




};