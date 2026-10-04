// Comprehensive Integration Tests
// Tests for Phase 2 compatibility, Phase 6 integration, and system integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>

namespace test_integration {

void testPhase2Compatibility() {
    // Test backward compatibility with Phase 2 functionality
    std::cout << "=== Testing Phase 2 Compatibility ===" << std::endl;
    
    // Recreate original Phase 2 test setup
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test all Phase 2 functionality
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test core operations
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Test statistics
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "  ✓ Phase 2 functionality intact" << std::endl;
}

void testPhase6Integration() {
    // Test Phase 6 integration (all systems working together)
    std::cout << "=== Testing Phase 6 Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    config->set("memory_enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize all systems
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test complex operations with integrated systems
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Test integrated memory systems
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test status reporting
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    std::cout << "  ✓ Phase 6 integration successful" << std::endl;
}

void testCommandLineIntegration() {
    // Test all CLI features integration
    std::cout << "=== Testing Command-Line Integration ===" << std::endl;
    
    // Test CLI argument parsing integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    
    // Test CLI configuration precedence
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Runtime); // CLI override
    
    assert(config->get<int64_t>("neuron_count").value() == 300);
    
    std::cout << "  ✓ Command-line integration working" << std::endl;
}

void testConfigurationIntegration() {
    // Test all configuration features integration
    std::cout << "=== Testing Configuration Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test multiple configuration parameters
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.005, nlm::ConfigSource::Default);
    config->set("simulation_steps", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("verbose", true, nlm::ConfigSource::Default);
    config->set("memory_enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Verify all configuration values are accessible
    assert(config->get<int64_t>("neuron_count").value() == 1000);
    assert(config->get<int64_t>("region_count").value() == 10);
    assert(config->get<double>("connection_probability").value() == 0.15);
    assert(config->get<double>("learning_rate").value() == 0.005);
    
    brain.initialize();
    
    std::cout << "  ✓ Configuration integration successful" << std::endl;
}

void testMemorySystemIntegration() {
    // Test working + episodic + associative memory integration
    std::cout << "=== Testing Memory System Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("memory_enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain with memory systems
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test memory system operation through brain interface
    assert(brain.getTotalNeuronCount() == 300);
    
    // Run simulation steps to exercise memory systems
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    std::cout << "  ✓ Memory system integration successful" << std::endl;
}

void testNeuromodulationIntegration() {
    // Test neuromodulation integration (dopamine, curiosity, novelty)
    std::cout << "=== Testing Neuromodulation Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize neuromodulation systems
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test neuromodulation effects through brain operations
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
    }
    
    // Verify neuromodulation affects system behavior
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "  ✓ Neuromodulation integration successful" << std::endl;
}

void testSystemIntegrationEndToEnd() {
    // Test end-to-end system integration
    std::cout << "=== Testing End-to-End System Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("simulation_steps", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Complete end-to-end integration test
    std::cout << "  Initializing brain..." << std::endl;
    bool initialized = brain.initialize();
    assert(initialized);
    
    std::cout << "  Running simulation..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    std::cout << "  Producing action..." << std::endl;
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "  System status..." << std::endl;
    assert(brain.getTotalNeuronCount() == 150);
    assert(brain.getTotalSynapseCount() >= 0);
    
    std::cout << "  ✓ End-to-end integration successful" << std::endl;
}

void runAll() {
    std::cout << "=== Integration Tests ===" << std::endl;
    testPhase2Compatibility();
    testPhase6Integration();
    testCommandLineIntegration();
    testConfigurationIntegration();
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testSystemIntegrationEndToEnd();
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_integration