//Holds individual training examples
#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <sstream>
#include "../data_container/data_container.hpp"
class Dataset {
private:
    std::vector<std::unique_ptr<DataContainer>> allContainers_;
    int totalContainers_ = 0;
    //Initalizes allContainers_
    void readCsvToContainers(const std::string& filePath, int featureLength);
public:
    int totalContainers() const  {
        return totalContainers_;
    }
    Dataset() {
        readCsvToContainers("./data/iris.data", 4);
    }
    Dataset(std::string filename, int nFeatures) {
        readCsvToContainers(filename, nFeatures);
    };
    const DataContainer& getContainer(int index) { return *allContainers_.at(index); }
};