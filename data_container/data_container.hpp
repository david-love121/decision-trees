#pragma once
#include <vector>
#include <string>
#include "../decision_tree/node.hpp"
class DataContainer {
private:
    int id_;
    std::vector<double> features_;
    std::string label_;
    Node<double>* currentNode;
public:
    DataContainer(int id, const std::vector<double>& features, const std::string& label)
        : id_(id), features_(features), label_(label) {}
    int getId() const { return id_; }
    const std::vector<double>& getFeatures() const { return features_; }
    const std::string& getLabel() const { return label_; }
    static int& idCounter() {
        static int counter = 0;
        return counter;
    }
    static int nextId() { return idCounter()++; }
    static void resetIdCounter(int start = 0) { idCounter() = start; }
};