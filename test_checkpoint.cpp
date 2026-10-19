// Test checkpoint save/load functionality
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <filesystem>
#include <cassert>

int main() {
    std::cout << "Testing checkpoint save/load functionality..." << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    
    // Create brain
    Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Run simulation for a few steps
    for (int i = 0; i < 10; ++i) {
        brain.step(i);
    }
    
    std::cout << "Ran 10 simulation steps" << std::endl;
    
    // Test checkpoint save
    std::string checkpointFile = "test_checkpoint.bin";
    if (!brain.save(checkpointFile)) {
        std::cerr << "Failed to save checkpoint" << std::endl;
        return 1;
    }
    
    std::cout << "Checkpoint saved successfully" << std::endl;
    
    // Create a new brain for loading
    Brain brain2(config);
    
    // Initialize the new brain
    if (!brain2.initialize()) {
        std::cerr << "Failed to initialize second brain" << std::endl;
        return 1;
    }
    
    // Test checkpoint load
    if (!brain2.load(checkpointFile)) {
        std::cerr << "Failed to load checkpoint" << std::endl;
        return 1;
    }
    
    std::cout << "Checkpoint loaded successfully" << std::endl;
    
    // Verify checkpoint was loaded correctly
    if (brain.getTotalNeuronCount() != brain2.getTotalNeuronCount()) {
        std::cerr << "Neuron count mismatch after loading checkpoint" << std::endl;
        return 1;
    }
    
    if (brain.getTotalSynapseCount() != brain2.getTotalSynapseCount()) {
        std::cerr << "Synapse count mismatch after loading checkpoint" << std::endl;
        return 1;
    }
    
    std::cout << "Checkpoint test passed successfully!" << std::endl;
    
    // Clean up
    std::filesystem::remove(checkpointFile);
    
    return 0;
}
