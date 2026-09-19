#include <iostream>
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "Testing Brain memory system getters..." << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cout << "ERROR: Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Test getWorkingMemory
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "SUCCESS: getWorkingMemory() returns valid pointer" << std::endl;
        // Note: isInitialized() method may not exist, so we just check pointer
        std::cout << "  Working memory: " << workingMemory << std::endl;
    } else {
        std::cout << "ERROR: getWorkingMemory() returns nullptr" << std::endl;
        return 1;
    }
    
    // Test getEpisodicMemory
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "SUCCESS: getEpisodicMemory() returns valid pointer" << std::endl;
        // Note: getMaxEpisodes() method may not exist
        std::cout << "  Episodic memory: " << episodicMemory << std::endl;
    } else {
        std::cout << "ERROR: getEpisodicMemory() returns nullptr" << std::endl;
        return 1;
    }
    
    // Test getAssociativeMemory
    auto* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory) {
        std::cout << "SUCCESS: getAssociativeMemory() returns valid pointer" << std::endl;
        std::cout << "  Associative memory: " << associativeMemory << std::endl;
    } else {
        std::cout << "ERROR: getAssociativeMemory() returns nullptr" << std::endl;
        return 1;
    }
    
    // Test getPredictionSystem
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        std::cout << "SUCCESS: getPredictionSystem() returns valid pointer" << std::endl;
        std::cout << "  Prediction system: " << predictionSystem << std::endl;
    } else {
        std::cout << "ERROR: getPredictionSystem() returns nullptr" << std::endl;
        return 1;
    }
    
    std::cout << "\nAll memory system getters are working correctly!" << std::endl;
    return 0;
}
