#include "core/Config/ConfigBuilder.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Configuration Builder Example ===\n" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    std::cout << "1. Using ConfigBuilder with presets..." << std::endl;
    
    // Example 1: Using predefined presets
    auto simpleConfig = nlm::ConfigBuilder::simple();
    auto standardConfig = nlm::ConfigBuilder::standard();
    auto researchConfig = nlm::ConfigBuilder::research();
    
    std::cout << "Simple config summary:" << std::endl;
    std::cout << simpleConfig.getConfig()->summary() << std::endl;
    
    std::cout << "2. Using fluent interface to build custom configuration..." << std::endl;
    
    // Example 2: Building custom configuration with fluent interface
    nlm::ConfigBuilder customBuilder;
    auto customConfig = customBuilder
        .neuronCount(750)
        .regionCount(3)
        .connectionProbability(0.12f)
        .simulationTimestep(0.001)
        .randomSeed(12345)
        .configureLIF(-75.0, -55.0, -75.0, 12.0)
        .configureSTDP(0.025f, 0.02f, 25.0f)
        .configureWorkingMemory(80, 0.015f)
        .configureNeuromodulation(0.8f, 0.3f)
        .getConfig();
    
    std::cout << "Custom config validation: " << (customConfig->summary().empty() ? "PASS" : "FAIL") << std::endl;
    
    std::cout << "3. Using Brain with ConfigBuilder..." << std::endl;
    
    // Example 3: Creating a brain with the new configuration system
    try {
        auto brainConfig = nlm::ConfigBuilder::demo();
        
        // Validate the configuration
        if (!brainConfig->validate()) {
            std::cout << "Configuration validation failed: " << brainConfig->summary() << std::endl;
            return 1;
        }
        
        // Create brain with configuration
        nlm::Brain brain(brainConfig);
        
        if (!brain.initialize()) {
            std::cout << "Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        std::cout << "Brain created successfully!" << std::endl;
        std::cout << "Brain status:" << std::endl;
        brain.logStatus();
        
        // Run a few simulation steps
        std::cout << "\nRunning 10 simulation steps..." << std::endl;
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step, step * 0.001);
        }
        
        std::cout << "Simulation completed successfully!" << std::endl;
        std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        
    } catch (const nlm::NLMException& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cout << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Configuration Builder Examples Complete ===" << std::endl;
    
    return 0;
}