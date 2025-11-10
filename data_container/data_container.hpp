#pragma once
#include <vector>
#include <string>

class DataContainer {
private:
    int id_;
    std::vector<double> features_;
    std::string label_;

public:
    DataContainer(const std::vector<double>& features, const std::string& label)
        : id_(nextId()), features_(features), label_(label) {}
    DataContainer(const DataContainer& other)
        : id_(other.id_), features_(other.features_), label_(other.label_) {}
    int getId() const { return id_; }
    const std::vector<double>& getFeatures() const { return features_; }
    const std::string& getLabel() const { return label_; }
    static int& idCounter() {
        static int counter = 0;
        return counter;
    }
     bool operator==(const DataContainer& other) const {
        return id_ == other.id_;
    }
    //friend declaration to overload ostream operator
    friend std::ostream& operator<<(std::ostream& os, const DataContainer& obj);
    static int nextId() { return idCounter()++; }
    static void resetIdCounter(int start = 0) { idCounter() = start; }
};
namespace std {
    template<>
    struct hash<DataContainer> {
        std::size_t operator()(const DataContainer& obj) const noexcept {
            //id is the datacontainer's unique identifer so we use this to hash
            std::size_t h1 = std::hash<int>{}(obj.getId());
            return h1;
        }
    };

}