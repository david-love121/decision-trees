#include "decision_tree.hpp"
#include "node.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <new>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include "../data_container/data_container.hpp"

std::vector<DataContainer> readCsvToContainers(const std::string& filePath = "./data/iris.data", int featureLength = 4) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open CSV file at " + filePath);
    }

    std::vector<DataContainer> containers;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::vector<double> features;
        std::stringstream ss(line);
        std::string cell;
        //Parse each item into an object which contains a feature vector of doubles and a classification string
        int i = 0;
        while (std::getline(ss, cell, ',') && i < featureLength) {
            i++;
            features.push_back(std::stod(cell));
        }
        DataContainer container(features, cell);
        containers.push_back(container);

        if (features.empty()) {
            throw std::runtime_error("Feature vector is empty after parsing line: " + line);
        }
    }

    return containers;
}
// Can be overloaded for other types, this works specifically for features = double. Returns ptr to last node
Node<double>* runTrainingExample(const DataContainer& container, Node<double>* head) {
    Node<double>* finishingContainer = head->runInput(container);
    return finishingContainer;
};
//Runs all examples, results are stored in the unordered map of each node
void runAllExamples(const std::vector<DataContainer>& containers, Node<double>* head) {
    
    for (int i = 0; i < containers.size(); i++) {
        runTrainingExample(containers.at(i), head);
    }
}
//Returns a map of where each container will land
std::unordered_map<DataContainer, Node<double>*> runAllExamplesMemoized(const std::vector<DataContainer>& containers, Node<double>* head) {
    std::unordered_map<DataContainer, Node<double>*> memoizedMap;
    for (int i = 0; i < containers.size(); i++) {
        Node<double>* finishContainer = runTrainingExample(containers.at(i), head); 
        memoizedMap[containers.at(i)] = finishContainer;
    }
    return memoizedMap;
}



int main() {
    //So I can easily change the type of the tree without updating every reference to it
    using dType = double;
    dType defaultValue = 0.0;
    Node<dType>* headNode = new Node<dType>(defaultValue);
    DecisionTree<dType> tree(headNode); 
    
    const std::vector<DataContainer> csvData = readCsvToContainers();
    const int nClassifications = 3;
    auto map = runAllExamplesMemoized(csvData, headNode);
    for (auto [key, value] : map) {
        
    }
    tree.calculateAllImpurity();
    auto featMap = tree.getHead()->getFeatureMap();
    double headImpurity = headNode->getImpurity();
    return 0;
}