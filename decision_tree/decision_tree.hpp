#pragma once

#include <map>
#include <memory>
#include "../dataset/dataset.hpp"
#include "./node.hpp"
template <typename T>
class DecisionTree {

public:
    //The constructors do not initialize a full tree - this is the responsibility of main.cpp
    explicit DecisionTree() : scoresFrozen_(false) {makeHeadNode(); } 


    const Node<T>& getHead() const { return nodeMap_[0].get(); }

    void runTree(T* input) { nodeMap_[0]->runInput(input); }
    void addNodes(std::unique_ptr<Node<T>> newNodeL, std::unique_ptr<Node<T>> newNodeR, int prevId) {nodeMap_[prevId]->setChildren(newNodeL, newNodeR); }
    void calculateAllImpurity() {
        nodeMap_[0]->calculateImpurityForward();
    }
    
    int getTotalNodes() { return nodeMap_.size(); }
    void resetLeafIds() { leafIds_.clear(); }
    void resetTree() {
        leafIds_.clear();
        nodeMap_[0]->resetNodeRecursive();
        scoresFrozen_ = false;
    }
    void makeHeadNode() {
        nodeMap_[0] = std::make_unique<Node<T>>();
    }
    const std::map<int, std::unique_ptr<Node<T>>> getNodeMap() const {
        return nodeMap_;
    }
    //Runs the tree
    void runTree() {
        
        for (int i = 0; i < dataset_.totalContainers(); i++) {
            int returnId = nodeMap_[0]->runInput(dataset_.getContainer(i));
            this->leafIds_.push_back(returnId);
        }

    }
    //Checks all the leaf nodes and splits them if necessary 
    void makeSplits() {
        for (int id : this->leafIds_) {
            double current_impurity = nodeMap_[id].getImpurity();
            //Make new children
            if (current_impurity != 0) {
                addNodes(std::make_unique<Node<T>>(T{}), std::make_unique<Node<T>>(T{}), id);
            }
            //Change split
            nodeMap_[id].setClassifierValue(nodeMap_[id].getAverageValue());
            //Todo: check if other features make better splits
        }

        
    }
private:
    std::map<int, std::unique_ptr<Node<T>>> nodeMap_;
    std::vector<int> leafIds_;
    Dataset dataset_;
    bool scoresFrozen_;
    
    

};