
#include "node.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <new>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include "decision_tree.hpp"

// Can be overloaded for other types, this works specifically for features = double. Returns ptr to last node

//Runs all examples, results are stored in the unordered map of each node



int main() {
    //So I can easily change the type of the tree without updating every reference to it
    using dType = double;

    DecisionTree<dType> tree; 
    


    int total = tree.getTotalNodes();
    tree.runTree();
    
    return 0;
}