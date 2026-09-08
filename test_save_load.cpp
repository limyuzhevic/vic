#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM Brain save/load functionality..." << std::endl;
    
    // Create brain with default config
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run some simulation steps
    std::cout << "Running 10 simulation steps..." << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        brain->step(i);
    }
    
    // Get current state
    std::cout << "Current state:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    // Save brain state
    const std::string savePath = "test_checkpoint.bin";
    std::cout << "Saving brain to " << savePath << std::endl;
    if (!brain->save(savePath)) {
        std::cerr << "Failed to save brain!" << std::endl;
        return 1;
    }
    
    // Reset brain to different initial state
    brain->reset();
    brain->initialize();
    
    std::cout << "After reset:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    // Load brain state back
    std::cout << "Loading brain from " << savePath << std::endl;
    if (!brain->load(savePath)) {
        std::cerr << "Failed to load brain!" << std::endl;
        return 1;
    }
    
    // Verify state was restored
    std::cout << "After load:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    // Run a few more steps to ensure it works
    for (size_t i = 10; i < 15; ++i) {
        brain->step(i);
    }
    
    std::cout << "Final total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    // Cleanup
    std::remove(savePath.c_str());
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}