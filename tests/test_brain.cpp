// Brain Tests
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace test_brain {

void testBrainValidation() {
    // Test config validation with invalid parameters
    auto config = std::make_shared<nlm::Config>();
    
    // Test invalid neuron count
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    // Should fail validation
    if (success) {
        throw std::runtime_error("Brain initialization should have failed with 0 neurons");
    }
    
    // Test invalid region count
    config->clear();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config);
    bool success2 = brain2.initialize();
    
    // Should fail validation
    if (success2) {
        throw std::runtime_error("Brain initialization should have failed with 0 regions");
    }
    
    // Test valid configuration
    config->clear();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain3(config);
    bool success3 = brain3.initialize();
    
    // Should succeed
    if (!success3) {
        throw std::runtime_error("Brain initialization should have succeeded with valid config");
    }
    
    assert(brain3.getRegionCount() == 1);
    assert(brain3.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainValidation passed" << std::endl;
}

void testBrainMemoryManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test reset functionality
    brain.step(0);
    brain.reset();
    
    // Verify brain is in valid state after reset
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getTotalSpikeCount() == 0);
    
    // Test re-initialization
    bool reinitSuccess = brain.initialize();
    assert(reinitSuccess);
    
    // Verify re-initialized brain is valid
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainMemoryManagement passed" << std::endl;
}

void testBrainErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test initialization with invalid config
    config->clear();
    config->set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config);
    bool success = brain2.initialize();
    
    // Should fail gracefully
    if (success) {
        throw std::runtime_error("Brain initialization should have failed with invalid config");
    }
    
    // Test with null config
    nlm::Brain brain3(nullptr);
    bool success2 = brain3.initialize();
    
    // Should fail gracefully
    if (success2) {
        throw std::runtime_error("Brain initialization should have failed with null config");
    }
    
    std::cout << "    testBrainErrorHandling passed" << std::endl;
}

void testBrainConfigValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test getting configuration values
    auto neuronCount = brain.getConfig()->get<int64_t>("neuron_count");
    assert(neuronCount && *neuronCount == 100);
    
    auto regionCount = brain.getConfig()->get<int64_t>("region_count");
    assert(regionCount && *regionCount == 1);
    
    auto connProb = brain.getConfig()->get<float>("connection_probability");
    assert(connProb && *connProb == 0.1f);
    
    // Test that brain has expected configuration
    assert(brain.getConfig()->has("neuron_count"));
    assert(brain.getConfig()->has("region_count"));
    assert(brain.getConfig()->has("connection_probability"));
    
    std::cout << "    testBrainConfigValidation passed" << std::endl;
}

} // namespace test_brain
