#include "Brain.hpp"

int main() {
    // Test that all memory accessors exist and are properly implemented
    std::cout << "Testing Brain memory accessors..." << std::endl;
    
    // These should all be accessible according to Brain.hpp
    // If any of these fail to compile, we know there's a mismatch
    
    // Create a mock config for testing
    auto config = std::make_shared<Config>();
    
    // Create brain instance
    Brain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test all memory accessors
    brain.getWorkingMemory();
    brain.getEpisodicMemory();
    brain.getSemanticMemory();
    brain.getProceduralMemory();
    brain.getAssociativeMemory();
    
    std::cout << "All memory accessors are accessible!" << std::endl;
    
    return 0;
}