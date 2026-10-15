// Basic neural network example
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <memory>
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set basic parameters
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    config->set("plasticity_learning_rate", 0.01);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully!" << std::endl;
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain->step(step);
        }
        
        // Get statistics
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Simulation completed successfully!" << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
}