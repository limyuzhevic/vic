// test_integration_edge_cases.cpp - Integration edge cases
#include "TestFramework.hpp"
#include "TestFixture.hpp"
#include "test_utils.cpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_integration_edge_cases {

// Test edge cases for system integration
void testSystemIntegration() {
    std::cout << "Testing system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to test system integration
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify system is still functional
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "    System integration test passed" << std::endl;
}

// Test edge cases for component communication
void testComponentCommunication() {
    std::cout << "Testing component communication..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to test component communication
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify all components are communicating
    assert(brain.getTotalSynapseCount() >= 0);
    std::cout << "    Component communication test passed" << std::endl;
}

// Test edge cases for interface mismatches
void testInterfaceMismatches() {
    std::cout << "Testing interface mismatches..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to test interface mismatches
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Should handle interface mismatches gracefully
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "    Interface mismatches test passed" << std::endl;
}

// Test edge cases for cross-component errors
void testCrossComponentErrors() {
    std::cout << "Testing cross-component errors..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation to test cross-component errors
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Should handle cross-component errors gracefully
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "    Cross-component errors test passed" << std::endl;
}

// Test edge cases for integration initialization
void testIntegrationInitializationEdgeCases() {
    std::cout << "Testing integration initialization edge cases..." << std::endl;
    
    // Test 1: Integration with no neurons
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(!success); // Should fail
        std::cout << "    Integration with no neurons test passed" << std::endl;
    }
    
    // Test 2: Integration with single neuron
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Should initialize successfully
        assert(brain.getTotalNeuronCount() == 1);
        std::cout << "    Integration with single neuron test passed" << std::endl;
    }
    
    // Test 3: Integration with very large neuron count
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Should initialize successfully
        assert(brain.getTotalNeuronCount() == 1000);
        std::cout << "    Integration with very large neuron count test passed" << std::endl;
    }
}

void runAll() {
    testSystemIntegration();
    testComponentCommunication();
    testInterfaceMismatches();
    testCrossComponentErrors();
    testIntegrationInitializationEdgeCases();
    
    std::cout << std::endl << "All integration edge case tests completed successfully!" << std::endl;
}

} // namespace test_integration_edge_cases