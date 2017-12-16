#include "Utility.hpp"

namespace utility {
    void printGraph(QueryGraph& graph) {
        std::cout << "Printing graph" << std::endl;

        for(auto pair : graph) {
            std::cout << "Name " << pair.first << " ";
            std::cout << "Relation: " << pair.second.first.relation_.getName() << std::endl;
            
            for (auto target : pair.second.second) {
                std::cout << "\tEdge to: " << target.connected_to_.relation_.getName() << std::endl;
            }
        }

    }
}