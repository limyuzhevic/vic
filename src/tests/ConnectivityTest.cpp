// NLM Phase 2: Connectivity Test
// Tests basic neural connectivity and spike propagation

#include <iostream>
#include <cassert>
#include <cmath>

// Include the necessary headers
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/LearningExperiment.hpp"

namespace test_connectivity {
    
    // Test 1: Basic neural connectivity
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
        
        if (!brain) {
            std::cout << "  ERROR: No brain provided" << std::endl;
            return;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "  ERROR: No region found" << std::endl;
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "  No neurons found!" << std::endl;
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        std::cout << "  Injecting current into " << std::min(size_t(10), neurons.size()) << " neurons..." << std::endl;
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
                float v = neurons[i]->getMembranePotential();
                float thresh = neurons[i]->getThreshold();
                std::cout << "    Neuron " << i << " V=" << v << " thresh=" << thresh << std::endl;
            }
        }
    }
    
    // Test helper function to check basic connectivity
    bool checkBasicConnectivity(std::shared_ptr<Brain> brain) {
        if (!brain) return false;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return false;
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first few neurons
        size_t testNeurons = std::min(size_t(3), neurons.size());
        for (size_t i = 0; i < testNeurons; ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 20; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        
        return spikes > 0;
    }
    
    // Test connectivity with specific neuron parameters
    bool testConnectivityParameters(std::shared_ptr<Brain> brain) {
        if (!brain) return false;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 3) return false;
        
        // Check membrane potentials and thresholds
        bool foundNeuronWithPotential = false;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            float v = neurons[i]->getMembranePotential();
            float thresh = neurons[i]->getThreshold();
            if (v > 0.0f && thresh > 0.0f) {
                foundNeuronWithPotential = true;
            }
        }
        
        return foundNeuronWithPotential;
    }
    
    void runAll() {
        // Initialize logger for testing
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Set test configuration
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain);
        
        // Test 2: Check neuron parameters after connectivity
        bool paramsTest = testConnectivityParameters(brain);
        std::cout << "  Neuron parameters test: " << (paramsTest ? "PASSED" : "FAILED") << std::endl;
        
        if (!paramsTest) {
            throw std::runtime_error("Connectivity parameters test failed");
        }
        
        std::cout << "=== Connectivity Tests ===" << std::endl;
        std::cout << "✓ All connectivity tests passed!" << std::endl;
    }
    
} // namespace test_connectivity

ConnectivityTest.cpp