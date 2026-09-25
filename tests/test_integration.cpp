// Integration Tests
// Tests for system integration including brain, memory, neuromodulation, and development systems
#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/WorkingMemory.hpp"
#include "development/DevelopmentSystem.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/MotorSystem.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "test_framework.hpp"

namespace test_integration {

// Integration test base class for system integration tests
class IntegrationTestBase : public nlm::test::TestBase {
protected:
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::Config> config;
    
public:
    IntegrationTestBase() {
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 100, nlm::ConfigSource::Default);
        config->set("region_count", 2, nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    }
    
    std::string getName() const override {
        return "IntegrationTestBase";
    }
    
    std::string getDescription() const override {
        return "Base class for integration tests";
    }
    
    nlm::test::TestCategory getCategory() const override {
        return nlm::test::TestCategory::Integration;
    }
    
    std::string getFile() const override {
        return "tests/integration/test_integration.cpp";
    }
    
    int getLine() const override {
        return 1;
    }
    
    void setup() override {
        brain = std::make_shared<nlm::Brain>(config);
        bool success = brain->initialize();
        assert(success);
    }
};

// Brain System Integration Tests
void testBrainSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test brain properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    // Test brain operations
    brain.step(0);
    brain.reset();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainSystemIntegration passed" << std::endl;
}

// Memory System Integration Tests
void testMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("working_memory_capacity", 100, nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 50, nlm::ConfigSource::Default);
    
    nlm::WorkingMemory workingMem(config);
    nlm::EpisodicMemory episodicMem(config);
    
    // Test memory initialization
    workingMem.store("test_data", "test_value", 1.0);
    
    std::string retrievedValue;
    double confidence = workingMem.retrieve("test_data", retrievedValue);
    
    assert(retrievedValue == "test_value");
    assert(confidence > 0.5);
    
    // Test episodic memory
    episodicMem.record("event1", 1.0, std::map<std::string, std::string>{
        {"location", "room1"}, {"context", "test_context"}
    });
    
    auto episodes = episodicMem.search("event1", 0.5);
    assert(!episodes.empty());
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

// Neuromodulation Integration Tests
void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_strength", 1.0, nlm::ConfigSource::Default);
    config->set("prediction_error", 0.5, nlm::ConfigSource::Default);
    config->set("novelty", 0.3, nlm::ConfigSource::Default);
    
    nlm::Neuromodulator neuromod(config);
    
    // Test neuromodulator initialization
    neuromod.update(0.0, 0, std::map<std::string, double>{});
    
    // Test reward processing
    neuromod.processReward(1.0, 0.0);
    
    // Test prediction error processing
    neuromod.processPredictionError(0.1, 0.5);
    
    // Test novelty detection
    neuromod.processNovelty(0.8, "test_pattern");
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

// Development System Integration Tests
void testDevelopmentSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("synaptogenesis_rate", 0.1, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.05, nlm::ConfigSource::Default);
    config->set("maturation_rate", 0.2, nlm::ConfigSource::Default);
    
    nlm::DevelopmentSystem devSystem(config);
    
    // Test synaptogenesis
    devSystem.processSynaptogenesis(0.0);
    
    // Test pruning
    devSystem.processPruning(0.0);
    
    // Test maturation
    devSystem.processMaturation(0.0);
    
    // Test overall development
    devSystem.processDevelopment(0.0);
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

// Multi-system Integration Tests
void testMultiSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    
    // Create and initialize multiple systems
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::WorkingMemory workingMem(config);
    nlm::Neuromodulator neuromod(config);
    nlm::DevelopmentSystem devSystem(config);
    
    // Simulate concurrent operations
    std::thread brainThread([&]() {
        for (int i = 0; i < 10; ++i) {
            brain.step(i);
        }
    });
    
    std::thread neuromodThread([&]() {
        for (int i = 0; i < 10; ++i) {
            neuromod.update(i * 0.1, i, {});
        }
    });
    
    brainThread.join();
    neuromodThread.join();
    
    // Test system consistency
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 2);
    
    std::cout << "    testMultiSystemIntegration passed" << std::endl;
}

// Comprehensive integration test suite
void runAll() {
    testBrainSystemIntegration();
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testDevelopmentSystemIntegration();
    testMultiSystemIntegration();
}

} // namespace test_integration
