#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Phase 6: Integrated Modular Architecture" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Add basic configuration
    config->set<size_t>("neuron_count", 100);
    config->set<size_t>("region_count", 2);
    config->set<size_t>("connection_probability", 0.1f);
    config->set<size_t>("simulation_timestep", 0.001);
    config->set<size_t>("random_seed", 12345);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Run a few simulation steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "Simulation steps completed" << std::endl;
    
    // Log brain status
    brain.logStatus();
    
    std::cout << "NLM completed successfully" << std::endl;
    return 0;
}