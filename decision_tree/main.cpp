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
        DataContainer container(DataContainer::nextId(), features, cell);
        containers.push_back(container);

        if (features.empty()) {
            throw std::runtime_error("Feature vector is empty after parsing line: " + line);
        }
    }

    return containers;
}
// Can be overloaded for other types, this works specifically for features = double. Returns the id of the last node
int runTrainingExample(const DataContainer& container, const Node<double>* head) {
    const std::vector<double> features = container.getFeatures();
    const Node<double>* finishingContainer = head->runInput(features);
    return finishingContainer->getId(); 
};
//Returns a list of IDs where the ID at index 0 is the ID where datacontainer at index 0 landed. For calculating gini impurity.
std::vector<int> runAllExamples(const std::vector<DataContainer>& containers, const Node<double>* head) {
    std::vector<int> listOfIds;
    for (int i = 0; i < containers.size(); i++) {
        int returnedId = runTrainingExample(containers.at(i), head);
        listOfIds.push_back(returnedId);
    }
    return listOfIds;
};

double calculateGiniImpurity(const std::vector<DataContainer>& containers, const std::vector<int> listOfIds, const int numberOfClassifications) {
    if (containers.size() != listOfIds.size()) {
        throw std::runtime_error("sizes do not match");
    }
    for (int i = 0; i < containers.size(); i++) {
        
    }
};

int main() {
    //So I can easily change the type of the tree without updating every reference to it
    using dType = double;
    dType defaultValue = 0.0;
    Node<dType>* headNode = new Node<dType>(defaultValue);
    DecisionTree<dType> tree(headNode); 
    Node<dType>* leftNode = new Node<dType>(defaultValue);
    Node<dType>* rightNode = new Node<dType>(defaultValue);
    headNode->setLeftChild(leftNode);
    headNode->setRightChild(rightNode); 
    const std::vector<DataContainer> csvData = readCsvToContainers();
    std::vector<int> listOfFinishingIds = runAllExamples(csvData, headNode);

    std::cout << Node<dType>::peekNextId();
    return 0;
}