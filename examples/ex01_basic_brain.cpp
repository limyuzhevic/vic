// ex01_basic_brain.cpp - Basic Brain Simulation

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== Example 1: Basic Brain Simulation ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 50);
    config->set("brain.learningRate", 0.01);
    config->set("brain.simulation.maxSteps", 500);
    config->set("brain.region.count", 2);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total regions: " << brain->getRegionCount() << std::endl;
    
    // Run simulation
    std::cout << "\nStarting simulation..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                     << brain->getActiveNeuronCount() << " active neurons, "
                     << brain->getFiringNeuronCount() << " firing neurons, "
                     << brain->getAverageFiringRate() << " Hz average rate" << std::endl;
        }
    }
    
    // Save brain state
    brain->save("ex01_checkpoint.brain");
    std::cout << "\nBrain state saved to 'ex01_checkpoint.brain'" << std::endl;
    
    std::cout << "=== Example 1 Complete ===" << std::endl;
    return 0;
}