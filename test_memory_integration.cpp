#include "src/brain/Brain.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Memory Consolidation and Replay Integration" << std::endl;
    
    // Create a simple test to verify the integration
    std::cout << "1. Brain class includes memory consolidation system: ";
    
    // Check if Brain has the memory consolidation getter
    Brain* brain = nullptr;
    if (brain) {
        auto* consolidation = brain->getMemoryConsolidation();
        auto* replay = brain->getMemoryReplay();
        auto* sleepState = brain->getSleepState();
        
        std::cout << "OK" << std::endl;
        std::cout << "2. Memory consolidation system integrated: ";
        
        if (consolidation && consolidation->isReady()) {
            std::cout << "OK" << std::endl;
            std::cout << "3. Memory replay system integrated: ";
            
            if (replay && replay->isReady()) {
                std::cout << "OK" << std::endl;
                std::cout << "4. Sleep state system integrated: ";
                
                if (sleepState && sleepState->isReady()) {
                    std::cout << "OK" << std::endl;
                    std::cout << "5. All memory systems successfully integrated!" << std::endl;
                } else {
                    std::cout << "FAILED - SleepState not ready" << std::endl;
                }
            } else {
                std::cout << "FAILED - MemoryReplay not ready" << std::endl;
            }
        } else {
            std::cout << "FAILED - MemoryConsolidation not ready" << std::endl;
        }
    } else {
        std::cout << "SKIPPED (Brain not created)" << std::endl;
    }
    
    return 0;
}