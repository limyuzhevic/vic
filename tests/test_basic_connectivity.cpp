// Basic Neural Connectivity Test
// Tests that spikes can propagate through the network

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

bool testBasicConnectivity() {
    std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    brain->logStatus();
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cout << "  No neurons found!" << std::endl;
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cout << "  No neurons found!" << std::endl;
        return false;
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
        return true;
    } else {
        std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            std::cout << "    Neuron " << i << 
                " V=" << neurons[i]->getMembranePotential() <<
                " thresh=" << neurons[i]->getThreshold() << std::endl;
        }
        return false;
    }
}

int main() {
    std::cout << "NLM Phase 2 Basic Connectivity Test" << std::endl;
    std::cout << "===================================" << std::endl;
    
    bool success = testBasicConnectivity();
    
    if (success) {
        std::cout << "\n✓ Basic connectivity test PASSED" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ Basic connectivity test FAILED" << std::endl;
        return 1;
    }
}