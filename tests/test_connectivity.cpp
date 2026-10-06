// Test 1: Basic Neural Connectivity Test
// Demonstrates that injected current produces spikes through the network

#include "Brain.hpp"
#include "Config/Config.hpp"
#include "Logger/Logger.hpp"
#include "ConsoleLogger/ConsoleLogger.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

void testBasicConnectivity() {
    std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
    
    // Create default configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Inject current into sensory neurons and observe spike propagation
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "No region found!" << std::endl;
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cerr << "No neurons found!" << std::endl;
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    std::cout << "  Injecting current into 10 neurons..." << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "  Spikes generated: " << spikes << std::endl;
    
    if (spikes > 0) {
        std::cout << "  ✓ Spikes propagate through network" << std::endl;
    } else {
        std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            std::cout << "    Neuron " << i 
                      << " V=" << neurons[i]->getMembranePotential()
                      << " thresh=" << neurons[i]->getThreshold() << std::endl;
        }
    }
}

int main() {
    testBasicConnectivity();
    return 0;
}
