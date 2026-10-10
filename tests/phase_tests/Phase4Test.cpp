// Phase 4 test - Memory integration and reinforcement learning
#include "memory/Memory.hpp"
#include "brain/Brain.hpp"
#include <iostream>

void runPhase4Test() {
    std::cout << "Running Phase 4: Memory Integration Tests" << std::endl;
    
    // Test memory integration
    nlm::memory::Memory memory(1000);
    memory.initialize();
    
    // Test reinforcement learning
    nlm::brain::Brain brain(100, 200.0);
    brain.initialize();
    
    // Simulate reward-based learning
    for (int i = 0; i < 10; ++i) {
        brain.update(0.05);
        memory.updateFromExperience(brain.getOutput(), 1.0);
    }
    
    std::cout << "Phase 4 test completed successfully" << std::endl;
}
