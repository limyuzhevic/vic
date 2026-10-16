// minimal_example.cpp
// Basic brain simulation example
// This is the simplest possible NLM example

#include <nlm/core/Config/Config.hpp>
#include <nlm/core/Random/Random.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <iostream>

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Minimal Example ===" << std::endl;
    
    // Create configuration with minimal settings
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 100, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    std::cout << "Creating brain with " << brain->getTotalNeuronCount() 
              << " neurons..." << std::endl;
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Run a few steps to observe activity
    std::cout << "Running simulation for 50 steps..." << std::endl;
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step);
        
        // Show progress
        if (step % 10 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " neurons firing, "
                      << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Excitatory/Inhibitory ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    return 0;
}
