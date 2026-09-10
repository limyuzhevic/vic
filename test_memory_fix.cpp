#include "Brain.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Neural Memory Systems..." << std::endl;
    
    // Create a simple config
    auto config = std::make_shared<Config>();
    
    // Create brain instance
    Brain brain(config);
    
    // Check if working memory is initialized
    NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "✓ Working memory is accessible" << std::endl;
        std::cout << "  - Active traces: " << workingMemory->getActiveTraces() << std::endl;
        std::cout << "  - Capacity: " << workingMemory->getCapacity() << std::endl;
        
        // Initialize the brain
        if (brain.initialize()) {
            std::cout << "✓ Brain initialization successful" << std::endl;
            
            // Check working memory after initialization
            NeuralWorkingMemory* workingMemoryAfter = brain.getWorkingMemory();
            if (workingMemoryAfter) {
                std::cout << "✓ Working memory still accessible after initialization" << std::endl;
                std::cout << "  - Active traces after init: " << workingMemoryAfter->getActiveTraces() << std::endl;
            }
            
            // Check episodic memory
            NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
            if (episodicMemory) {
                std::cout << "✓ Episodic memory is accessible" << std::endl;
                std::cout << "  - Episode count: " << episodicMemory->getEpisodeCount() << std::endl;
            }
            
            // Check associative memory
            NeuralAssociativeMemory* associativeMemory = brain.getAssociativeMemory();
            if (associativeMemory) {
                std::cout << "✓ Associative memory is accessible" << std::endl;
            }
            
        } else {
            std::cout << "✗ Brain initialization failed" << std::endl;
        }
    } else {
        std::cout << "✗ Working memory is nullptr" << std::endl;
    }
    
    return 0;
}
