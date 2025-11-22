#pragma once
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "../data_container/data_container.hpp"
#include "dataset/dataset.hpp"
//originally was using templates but realized doubles throughout is smarter  
class Node {
private:
    int id_;
    // The value which accepts, or sends to the right node when input >= value
    double classifierValue_;
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

    static int& idCounter() {
        static int counter = 0;
        return counter;
    }
    static int nextId() { return idCounter()++; }
    void resetSamples() { nSamples_ = 0; }
    void resetSampleIndices() { sampleIndices_.clear(); }
    void resetClassCounts() { classCounts_.clear(); }
public:
    static int peekNextId() { return idCounter(); }

    explicit Node(double value = 0.0)
        : id_(nextId()), classifierValue_(value), leftChild_(nullptr), rightChild_(nullptr), featureIndex_(0), impurity_(0.0), frozen_(true), nSamples_(0) {

        }

    const int getId() const { return id_; }
    const bool getIsLeaf() const { return leftChild_ == nullptr && rightChild_ == nullptr; } 
    const double getClassifierValue() const { return classifierValue_; }

    const int getFeatureIndex() const { return featureIndex_; }
    const double getImpurity() {
        return frozen_ ? impurity_ : calculateImpurityScore();
    }
    const int getNumberSamples() const {
        return nSamples_;
    }

    void setClassifierValue(double value) { classifierValue_ = value; }
    void setFeatureIndex(int newIndex) {featureIndex_ = newIndex; }

    //Increments and returns new value
    int incrementSamples() { nSamples_++; return nSamples_; }

    //returns the node which the container finishes on
    const int runInput(const DataContainer& container) {
        frozen_ = false;

        std::vector<double> features = container.getFeatures();
        std::string label = container.getLabel();
        int currentNodeId = this->id_;
        incrementSamples();
        sampleIndices_.push_back(container.getId());
        classCounts_.emplace(container.getLabel(), 0);
        classCounts_[container.getLabel()]++;
        if (this->getIsLeaf()) {    
            return currentNodeId;
        }

        double input = features.at(featureIndex_);

        if (input >= classifierValue_) {
            currentNodeId = rightChild_->runInput(container);
        } else {
            currentNodeId = leftChild_->runInput(container);
        }
        return currentNodeId;
    }
    //Calculates impurity score of all nodes, returns leaf impurities
    double calculateImpurityForward() {
        frozen_ = false;        
        double thisImpurity = getImpurity();
        if (this->getIsLeaf()) {
            return thisImpurity;
        }
        double leftImpurity = this->leftChild_->calculateImpurityForward();
        double rightImpurity = this->rightChild_->calculateImpurityForward();
        return (leftImpurity + rightImpurity) / 2;
    }
    void resetNode() {
        this->resetSampleIndices();
        this->resetSamples();
        this->resetClassCounts();
        this->frozen_ = false;
    }
    void resetNodeRecursive() {
        this->resetNode();
        if (getIsLeaf()) {
            return;
        }
        this->leftChild_->resetNodeRecursive();
        this->rightChild_->resetNodeRecursive();
    }
    //Split this node 
    void createSplit() {
        if (!this->getIsLeaf()) {
            throw std::runtime_error("Node should be a leaf"); //Something is wrong! Node should be a leaf when this is called
        }
        this->leftChild_ = std::make_unique<Node>(Node());
        this->rightChild_ = std::make_unique<Node>(Node());
    }
    //Try selecting a different classifier value / feature
    //epsilon is the arbitrary precision of the search
    void optimizeNode(const Dataset& dataset) {
        if (sampleIndices_.size() == 0) {
            std::cout << "Warning: tried to split a node with no samples, skipping";
            return;
        }
        if (!this->getIsLeaf()) {
            this->leftChild_->optimizeNode(dataset);
            this->rightChild_->optimizeNode(dataset);
            return;
        }
        int nFeatures = dataset.getContainer(0).getFeatures().size();
        double bestImpurity = this->getImpurity();
        int bestFeatureIndex = this->getFeatureIndex();
        double bestSplitValue = this->getClassifierValue();
        bool foundBetterSplit = false;
        for (int i = 0; i < nFeatures; i++) {
            //Pairwise compare midpoints for better splits
            std::vector<std::pair<double, std::string>> featureLabels;
            featureLabels.reserve(sampleIndices_.size());
            for (auto idx : sampleIndices_) {
                const DataContainer& thisContainer = dataset.getContainer(idx);
                featureLabels.push_back({thisContainer.getFeatures()[i], thisContainer.getLabel()});
            }
            //sort
            std::sort(featureLabels.begin(), featureLabels.end(), [](const auto& a, const auto& b) {return a.first < b.first;});

            // 3. Linear scan to find best split
            // Start with all samples on the right
            std::unordered_map<std::string, int> leftCounts;
            std::unordered_map<std::string, int> rightCounts = this->classCounts_;
            
            int leftTotal = 0;
            int rightTotal = this->nSamples_;

            for (size_t k = 0; k < featureLabels.size() - 1; k++) {
                const auto& val = featureLabels[k];
                const auto& nextVal = featureLabels[k+1];
                const std::string& label = val.second;

                // Move sample from Right to Left
                rightCounts[label]--;
                leftCounts[label]++;
                leftTotal++;
                rightTotal--;

                // If adjacent values are identical, we cannot split between them
                if (val.first == nextVal.first) continue;

                // Calculate Gini for Left
                double giniLeft = 1.0;
                for (const auto& [lbl, count] : leftCounts) {
                    if (count > 0) {
                        double prob = (double)count / leftTotal;
                        giniLeft -= prob * prob;
                    }
                }

                // Calculate Gini for Right
                double giniRight = 1.0;
                for (const auto& [lbl, count] : rightCounts) {
                    if (count > 0) {
                        double prob = (double)count / rightTotal;
                        giniRight -= prob * prob;
                    }
                }

                // Weighted Gini Impurity of the split
                double weightedImpurity = ((double)leftTotal / nSamples_) * giniLeft + 
                                          ((double)rightTotal / nSamples_) * giniRight;

                if (weightedImpurity < bestImpurity) {
                    bestImpurity = weightedImpurity;
                    bestFeatureIndex = i;
                    bestSplitValue = (val.first + nextVal.first) / 2.0;
                    foundBetterSplit = true;
                }
            }
        }

        if (foundBetterSplit) {
            this->setFeatureIndex(bestFeatureIndex);
            this->setClassifierValue(bestSplitValue);
            //recalculate parent impurity
            this->calculateImpurityScore();
            this->createSplit();
        }
    return;
    }

    
private:
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



};