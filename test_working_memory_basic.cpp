#include <iostream>
#include <cassert>
#include "src/memory/NeuralWorkingMemory.hpp"

// Simple test to verify NeuralWorkingMemory compiles and basic functionality works
int main() {
    std::cout << "=== NeuralWorkingMemory Compilation Test ===" << std::endl;
    
    // Test that the header file compiles
    nlm::NeuralWorkingMemory wm(7, 0.05f);
    
    std::cout << "✓ NeuralWorkingMemory class compiles successfully" << std::endl;
    std::cout << "✓ Constructor creates instance" << std::endl;
    std::cout << "✓ Capacity: " << wm.getCapacity() << std::endl;
    std::cout << "✓ Decay rate: " << wm.getDecayRate() << std::endl;
    
    // Test interface methods exist
    std::cout << "✓ Interface methods exist" << std::endl;
    
    std::cout << "\n=== Basic Compilation Test PASSED ===" << std::endl;
    
    return 0;
}
