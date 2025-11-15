#include "dataset.hpp"

void Dataset::readCsvToContainers(const std::string& filePath = "./data/iris.data", int featureLength = 4) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open CSV file at " + filePath);
    }

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
        std::unique_ptr<DataContainer> container = std::make_unique<DataContainer>(DataContainer(features, cell));
        this->allContainers_.push_back(std::move(container));
        totalContainers_++;
        if (features.empty()) {
            throw std::runtime_error("Feature vector is empty after parsing line: " + line);
        }
    }

    return;
}