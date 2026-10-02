#include <iostream>
#include <string>
#include <vector>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

// Simple test program to verify improvements
int main() {
    std::cout << "Testing NLM Improvements" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Test 1: Configuration validation
    std::cout << "\n1. Testing Configuration Validation:" << std::endl;
    nlm::Config config;
    
    // Set valid values
    config.set("random_seed", 42);
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", 1000);
    config.set("connection_probability", 0.15);
    
    // Set invalid values (should generate warnings)
    config.set("random_seed", -1);  // Invalid - negative seed
    config.set("connection_probability", 1.5);  // Invalid - > 1.0
    
    // Validate configuration
    auto errors = config.validate();
    std::cout << "Validation found " << errors.size() << " warnings/errors:" << std::endl;
    for (const auto& error : errors) {
        std::cout << "  - " << error << std::endl;
    }
    
    // Test 2: Configuration file path
    std::cout << "\n2. Testing File Path Storage:" << std::endl;
    std::string filePath = config.getFilePath();
    std::cout << "File path: " << filePath << std::endl;
    
    // Test 3: Configuration summary
    std::cout << "\n3. Testing Configuration Summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test 4: Brain initialization
    std::cout << "\n4. Testing Brain Initialization:" << std::endl;
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (brain->initialize()) {
        std::cout << "✓ Brain initialized successfully" << std::endl;
        
        // Test 5: Basic connectivity
        std::cout << "\n5. Testing Basic Connectivity:" << std::endl;
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            std::cout << "  Found " << neurons.size() << " neurons" << std::endl;
            
            // Get initial spike count
            size_t initialSpikes = brain->getTotalSpikeCount();
            std::cout << "  Initial spike count: " << initialSpikes << std::endl;
            
            // Inject current into neurons
            for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            // Run simulation
            for (int step = 0; step < 10; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t finalSpikes = brain->getTotalSpikeCount();
            std::cout << "  Final spike count: " << finalSpikes << std::endl;
            std::cout << "  Spikes generated: " << (finalSpikes - initialSpikes) << std::endl;
        } else {
            std::cout << "  ! No regions found" << std::endl;
        }
        
        brain->reset();
        brain->initialize();
    } else {
        std::cout << "✗ Failed to initialize brain" << std::endl;
    }
    
    std::cout << "\n" << std::endl;
    std::cout << "All tests completed!" << std::endl;
    
    return 0;
}