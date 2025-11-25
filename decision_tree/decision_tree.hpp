#pragma once

#include <memory>
#include "../dataset/dataset.hpp"
#include "./node.hpp"

class DecisionTree {

private:
    std::unique_ptr<Node> head_;
    Dataset dataset_;
    static int totalNodes_;
    static int getNextId() {
        totalNodes_ += 1;
        return totalNodes_;
    }
  
public:

    explicit DecisionTree() { makeHeadNode(); }   
    static int getTotalNodes() {
        return totalNodes_;
    }
    const Node* getHeadNode() const { return head_.get(); }
    Node* getHeadNode() { return head_.get(); }
    const Dataset& getDataset() const { return dataset_; }

    void runTree(DataContainer& input) { head_->runInput(input); }
    double calculateAllImpurity() {
        return head_->calculateImpurityForward();
        
    }
    
    void resetTree() {
        head_->resetNodeRecursive();

    }
    void makeHeadNode() {
        head_ = std::make_unique<Node>(Node());
    }

    //Runs the tree oiver the dataset
    void runTree() {
        resetTree();
        for (int i = 0; i < dataset_.totalContainers(); i++) {
            head_->runInput(dataset_.getContainer(i));
        }
    }

    //Recursive split
    void makeSplits() {
        this->head_->optimizeNode(dataset_);
        //Todo: Finish
        
        
    }

    //Runs an iteration of the training loop, calculates aggregate impurity before and ater 



};
