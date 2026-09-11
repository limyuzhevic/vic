// Test to verify the actual integration status
#include "Brain.hpp"
#include <iostream>

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    Brain brain(config);
    brain.initialize();
    
    std::cout << "=== BRAIN INTEGRATION STATUS ===" << std::endl;
    std::cout << "getWorkingMemory(): " << (brain.getWorkingMemory() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getEpisodicMemory(): " << (brain.getEpisodicMemory() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getAssociativeMemory(): " << (brain.getAssociativeMemory() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getPredictionSystem(): " << (brain.getPredictionSystem() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getPlanner(): " << (brain.getPlanner() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getConceptFormation(): " << (brain.getConceptFormation() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getAttention(): " << (brain.getAttention() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getDevelopmentSystem(): " << (brain.getDevelopmentSystem() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getDopamine(): " << (brain.getDopamine() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getCuriosity(): " << (brain.getCuriosity() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getNovelty(): " << (brain.getNovelty() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "getPredictionErrorSignal(): " << (brain.getPredictionErrorSignal() ? "NOT NULL" : "NULL") << std::endl;
    
    return 0;
}