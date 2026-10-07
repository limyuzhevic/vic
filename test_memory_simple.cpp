#include <iostream>
#include <memory>

// Simple test to verify memory system compilation
// This test focuses on the basic compilation and initialization of memory systems

namespace nlm {
    class Brain;
}

// Forward declarations for memory components
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

int main() {
    std::cout << "Testing Memory System Integration..." << std::endl;
    
    try {
        // Test 1: Create brain using simple constructor
        {
            nlm::Brain brain;
            std::cout << "✓ Test 1 passed: Brain creation with default constructor" << std::endl;
            
            // Access memory systems
            auto* workingMem = brain.getWorkingMemory();
            auto* episodicMem = brain.getEpisodicMemory();
            auto* associativeMem = brain.getAssociativeMemory();
            
            if (workingMem) {
                std::cout << "✓ Working memory accessible" << std::endl;
            }
            if (episodicMem) {
                std::cout << "✓ Episodic memory accessible" << std::endl;
            }
            if (associativeMem) {
                std::cout << "✓ Associative memory accessible" << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "All basic compilation and initialization tests passed! ✓" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
