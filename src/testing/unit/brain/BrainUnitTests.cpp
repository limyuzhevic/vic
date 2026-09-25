// src/testing/unit/brain/BrainUnitTests.cpp
// Unit tests for brain system

#include "Brain.hpp"
#include "BrainFactory.hpp"
#include "Config.hpp"
#include "ConfigFactory.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>

namespace nlm {

class BrainUnitTests {
public:
    static void testBrainCreation() {
        std::cout << "Testing brain creation..." << std::endl;
        
        auto config = ConfigFactory::createConfig();
        config->set("neuron_count", 100, ConfigSource::User);
        
        auto brain = BrainFactory::createBrain(config);
        assert(brain != nullptr);
        assert(brain->getNeuronCount() == 100);
        
        std::cout << "✓ Brain creation test passed" << std::endl;
    }
    
    static void testBrainInitialization() {
        std::cout << "Testing brain initialization..." << std::endl;
        
        auto config = ConfigFactory::createConfig();
        config->set("neuron_count", 50, ConfigSource::User);
        
        auto brain = BrainFactory::createBrain(config);
        assert(brain != nullptr);
        
        brain->initialize();
        assert(brain->isInitialized());
        
        std::cout << "✓ Brain initialization test passed" << std::endl;
    }
    
    static void testBrainReset() {
        std::cout << "Testing brain reset..." << std::endl;
        
        auto config = ConfigFactory::createConfig();
        config->set("neuron_count", 75, ConfigSource::User);
        
        auto brain = BrainFactory::createBrain(config);
        assert(brain != nullptr);
        
        brain->initialize();
        brain->reset();
        
        // Brain should still be valid after reset
        assert(brain->getNeuronCount() == 75);
        
        std::cout << "✓ Brain reset test passed" << std::endl;
    }
    
    static void testNeuronConnectivity() {
        std::cout << "Testing neuron connectivity..." << std::endl;
        
        auto config = ConfigFactory::createConfig();
        config->set("neuron_count", 200, ConfigSource::User);
        config->set("connection_probability", 0.1, ConfigSource::User);
        
        auto brain = BrainFactory::createBrain(config);
        assert(brain != nullptr);
        
        brain->initialize();
        
        // Check that neurons have connections
        size_t totalConnections = 0;
        for (const auto& region : brain->getRegions()) {
            for (const auto& population : region->getPopulations()) {
                for (const auto& neuron : population->getNeurons()) {
                    totalConnections += neuron->getConnectionCount();
                }
            }
        }
        
        // With 200 neurons and 0.1 connection probability, we expect some connections
        assert(totalConnections > 0);
        
        std::cout << "✓ Neuron connectivity test passed" << std::endl;
    }
    
    static void testBrainSimulationStep() {
        std::cout << "Testing brain simulation step..." << std::endl;
        
        auto config = ConfigFactory::createConfig();
        config->set("neuron_count", 100, ConfigSource::User);
        config->set("simulation_timestep", 0.1, ConfigSource::User);
        
        auto brain = BrainFactory::createBrain(config);
        assert(brain != nullptr);
        
        brain->initialize();
        
        // Run a few simulation steps
        for (int i = 0; i < 10; ++i) {
            brain->simulateStep();
        }
        
        // Brain should still be valid after simulation
        assert(brain->getNeuronCount() == 100);
        assert(brain->isInitialized());
        
        std::cout << "✓ Brain simulation step test passed" << std::endl;
    }
};

}} // namespace nlm