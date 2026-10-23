// Test the refactored BrainRefactored class
// Simple demonstration that the refactoring preserves functionality

#include <iostream>
#include <memory>
#include "BrainRefactored.hpp"

int main() {
    std::cout << "=== Testing Refactored BrainRefactored ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Configure basic parameters
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42);
    
    // Create brain
    nlm::BrainRefactored brain(config);
    
    // Initialize brain
    std::cout << "Initializing brain..." << std::endl;
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Test basic statistics
    std::cout << "Brain statistics:" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Regions: " << brain.getRegionCount() << std::endl;
    std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << std::endl;
    
    // Test simulation steps
    std::cout << "\nRunning simulation..." << std::endl;
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
        std::cout << "  Step " << step << ": " << brain.getFiringNeuronCount() << " firing neurons" << std::endl;
    }
    
    // Test action production
    std::cout << "\nTesting action production..." << std::endl;
    auto action = brain.produceAction();
    std::cout << "  Action type: " << static_cast<int>(action->getType()) << std::endl;
    
    // Test development
    std::cout << "\nTesting development..." << std::endl;
    brain.develop();
    std::cout << "  Developmental stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    
    // Test status logging
    std::cout << "\nBrain status:" << std::endl;
    brain.logStatus();
    
    std::cout << "\n=== All tests passed ===" << std::endl;
    return 0;
}
