// Tutorial 1: Creating Your First Brain
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "Tutorial 1: Creating Your First Brain" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("plasticity_learning_rate", 0.01);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (brain->initialize()) {
        std::cout << "✓ Brain initialized successfully!" << std::endl;
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
            
            if (step % 20 == 0) {
                std::cout << "Step " << step << ": " << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
            }
        }
        
        std::cout << "✓ Simulation completed successfully!" << std::endl;
        std::cout << "✓ Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "✓ Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        return 0;
    } else {
        std::cerr << "✗ Failed to initialize brain!" << std::endl;
        return 1;
    }
}