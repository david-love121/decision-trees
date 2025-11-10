#include <iostream>
#include "data_container.hpp"
std::ostream& operator<<(std::ostream& os, const DataContainer& container) {
    os << "DataContainer{id: " << container.getId() << ", features: [";
    const auto& feats = container.getFeatures();
    for (size_t i = 0; i < feats.size(); ++i) {
        if (i) os << ", ";
        os << feats[i];
    }
    os << "], label: " << container.getLabel() << '}';
    return os;
}