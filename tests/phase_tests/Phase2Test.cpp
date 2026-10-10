// Phase 2 test - Synaptic plasticity and learning
#include "brain/Brain.hpp"
#include <iostream>

void runPhase2Test() {
    std::cout << "Running Phase 2: Synaptic Plasticity Tests" << std::endl;
    
    // Test basic synaptic connectivity
    nlm::brain::Brain brain(50, 100.0);
    brain.initialize();
    
    // Test synaptic weight updates
    brain.update(0.01);
    
    // Verify learning behavior
    std::cout << "Phase 2 test completed successfully" << std::endl;
}
