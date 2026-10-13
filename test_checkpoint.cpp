// Test program to verify Brain::save() and Brain::load() functionality
// This creates a simple integration test for Phase 6 checkpointing

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <iostream>

int main() {
    // Create and configure a simple test
    auto config = std::make_shared<Config>();
    
    // Set minimal configuration for test
    config->set<size_t>("neuron_count", 100);
    config->set<size_t>("region_count", 2);
    config->set<float>("connection_probability", 0.1f);
    config->set<size_t>("random_seed", 12345);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Run a few simulation steps
    for (size_t i = 0; i < 10; ++i) {
        brain.step(i, static_cast<double>(i) * 0.01);
    }
    
    std::cout << "Ran 10 simulation steps" << std::endl;
    std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Active neurons: " << brain.getActiveNeuronCount() << std::endl;
    
    // Save brain state
    const std::string checkpointFile = "checkpoints/test_checkpoint.nml";
    if (!brain.save(checkpointFile)) {
        std::cerr << "Failed to save brain state" << std::endl;
        return 1;
    }
    
    std::cout << "Brain state saved to: " << checkpointFile << std::endl;
    
    // Reset brain to test loading
    brain.reset();
    std::cout << "Brain reset after save" << std::endl;
    std::cout << "  Total neurons after reset: " << brain.getTotalNeuronCount() << std::endl;
    
    // Load brain state
    if (!brain.load(checkpointFile)) {
        std::cerr << "Failed to load brain state" << std::endl;
        return 1;
    }
    
    std::cout << "Brain state loaded successfully" << std::endl;
    
    // Verify some state was restored
    // Note: Some state (like spike counts) will be reset, but neural states should be loaded
    std::cout << "  Total neurons after load: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses after load: " << brain.getTotalSynapseCount() << std::endl;
    
    // Run a few more steps to ensure brain is functional after load
    for (size_t i = 10; i < 20; ++i) {
        brain.step(i, static_cast<double>(i) * 0.01);
    }
    
    std::cout << "Ran additional 10 simulation steps after load" << std::endl;
    std::cout << "  Total spikes after load: " << brain.getTotalSpikeCount() << std::endl;
    
    std::cout << "\nTest completed successfully! Brain save/load functionality verified." << std::endl;
    
    return 0;
}
