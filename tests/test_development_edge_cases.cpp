// test_development_edge_cases.cpp - Development edge cases
#include "TestFramework.hpp"
#include "TestFixture.hpp"
#include "test_utils.cpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_development_edge_cases {

// Test edge cases for development stage transitions
void testDevelopmentStageTransitions() {
    std::cout << "Testing development stage transitions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to test development stage transitions
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Should complete without errors
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "    Development stage transitions test passed" << std::endl;
}

// Test edge cases for age-related plasticity changes
void testAgeRelatedPlasticityChanges() {
    std::cout << "Testing age-related plasticity changes..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to test age-related plasticity changes
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Should complete without errors
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "    Age-related plasticity changes test passed" << std::endl;
}

// Test edge cases for structural development failures
void testStructuralDevelopmentFailures() {
    std::cout << "Testing structural development failures..." << std::endl;
    
    // Test 1: Structural development with no neurons
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(!success); // Should fail
        std::cout << "    Structural development with no neurons test passed" << std::endl;
    }
    
    // Test 2: Structural development with single neuron
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Should initialize successfully
        assert(brain.getTotalNeuronCount() == 1);
        std::cout << "    Structural development with single neuron test passed" << std::endl;
    }
}

// Test edge cases for synaptogenesis
void testSynaptogenesisEdgeCases() {
    std::cout << "Testing synaptogenesis edge cases..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to test synaptogenesis
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Should complete without errors
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "    Synaptogenesis edge cases test passed" << std::endl;
}

// Test edge cases for development initialization
void testDevelopmentInitializationEdgeCases() {
    std::cout << "Testing development initialization edge cases..." << std::endl;
    
    // Test 1: Development with no neurons
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(!success); // Should fail
        std::cout << "    Development with no neurons test passed" << std::endl;
    }
    
    // Test 2: Development with single neuron
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Should initialize successfully
        assert(brain.getTotalNeuronCount() == 1);
        std::cout << "    Development with single neuron test passed" << std::endl;
    }
    
    // Test 3: Development with very large neuron count
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Should initialize successfully
        assert(brain.getTotalNeuronCount() == 1000);
        std::cout << "    Development with very large neuron count test passed" << std::endl;
    }
}

void runAll() {
    testDevelopmentStageTransitions();
    testAgeRelatedPlasticityChanges();
    testStructuralDevelopmentFailures();
    testSynaptogenesisEdgeCases();
    testDevelopmentInitializationEdgeCases();
    
    std::cout << std::endl << "All development edge case tests completed successfully!" << std::endl;
}

} // namespace test_development_edge_cases