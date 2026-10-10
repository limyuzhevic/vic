// test_plasticity_edge_cases.cpp - Plasticity edge cases
#include "TestFramework.hpp"
#include "TestFixture.hpp"
#include "test_utils.cpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_plasticity_edge_cases {

// Test edge cases for zero synapses
void testZeroSynapses() {
    std::cout << "Testing zero synapses..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.0, nlm::ConfigSource::Default); // No connections
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Should have zero synapses
    assert(brain.getTotalSynapseCount() == 0);
    std::cout << "    Zero synapses test passed" << std::endl;
}

// Test edge cases for single neuron
void testSingleNeuron() {
    std::cout << "Testing single neuron..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 1.0, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Single neuron should have no synapses
    assert(brain.getTotalNeuronCount() == 1);
    assert(brain.getTotalSynapseCount() == 0);
    std::cout << "    Single neuron test passed" << std::endl;
}

// Test edge cases for full connectivity
void testFullConnectivity() {
    std::cout << "Testing full connectivity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 1.0, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // With full connectivity, should have max possible synapses
    // (10 neurons * (10-1) possible connections per neuron / 2 for undirected)
    int expectedMaxSynapses = (10 * 9) / 2; // Assuming symmetric connections
    assert(brain.getTotalSynapseCount() == expectedMaxSynapses);
    std::cout << "    Full connectivity test passed" << std::endl;
}

// Test edge cases for extreme weight values
void testExtremeWeightValues() {
    std::cout << "Testing extreme weight values..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("learning_rate", 1000.0, nlm::ConfigSource::Default); // Extreme learning rate
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to see if extreme weights cause issues
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Should not crash or produce NaN/Inf values
    assert(brain.getAverageFiringRate() >= 0.0f);
    std::cout << "    Extreme weight values test passed" << std::endl;
}

// Test edge cases for learning rate
void testLearningRateEdgeCases() {
    std::cout << "Testing learning rate edge cases..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Test very small learning rate
    config->set("learning_rate", 0.000001, nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Test very large learning rate
    config->set("learning_rate", 100.0, nlm::ConfigSource::Default);
    brain.reset();
    brain.initialize();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    std::cout << "    Learning rate edge cases test passed" << std::endl;
}

// Test edge cases for plasticity rule combinations
void testPlasticityRuleCombinations() {
    std::cout << "Testing plasticity rule combinations..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to test plasticity rule combinations
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    
    // Should have stable learning behavior
    assert(brain.getAverageFiringRate() >= 0.0f);
    std::cout << "    Plasticity rule combinations test passed" << std::endl;
}

// Test edge cases for plasticity initialization
void testPlasticityInitializationEdgeCases() {
    std::cout << "Testing plasticity initialization edge cases..." << std::endl;
    
    // Test 1: Plasticity with no neurons
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(!success); // Should fail
        std::cout << "    Plasticity with no neurons test passed" << std::endl;
    }
    
    // Test 2: Plasticity with single neuron
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Single neuron should have no synapses
        assert(brain.getTotalSynapseCount() == 0);
        std::cout << "    Plasticity with single neuron test passed" << std::endl;
    }
    
    // Test 3: Plasticity with very large neuron count
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Should initialize successfully
        assert(brain.getTotalNeuronCount() == 1000);
        std::cout << "    Plasticity with very large neuron count test passed" << std::endl;
    }
}

void runAll() {
    testZeroSynapses();
    testSingleNeuron();
    testFullConnectivity();
    testExtremeWeightValues();
    testLearningRateEdgeCases();
    testPlasticityRuleCombinations();
    testPlasticityInitializationEdgeCases();
    
    std::cout << std::endl << "All plasticity edge case tests completed successfully!" << std::endl;
}

} // namespace test_plasticity_edge_cases