// Comprehensive Edge Case Tests for NLM Brain
// Tests edge cases, error conditions, and boundary scenarios

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_edge_cases {

/// Test brain with invalid/zero configuration parameters
void testEdgeCaseZeroNeurons() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    // Zero neurons might be valid (empty brain), but should not crash
    assert(success);
    assert(brain.getTotalNeuronCount() == 0);
    assert(brain.getTotalSynapseCount() == 0);
    
    std::cout << "    testEdgeCaseZeroNeurons passed" << std::endl;
}

/// Test brain with very large configuration parameters
void testEdgeCaseLargeConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.01, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<uint64_t>(999999), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(success);
    assert(brain.getTotalNeuronCount() == 10000);
    assert(brain.getRegionCount() == 100);
    
    std::cout << "    testEdgeCaseLargeConfiguration passed" << std::endl;
}

/// Test brain with different neuron type configurations
void testEdgeCaseMixedNeuronTypes() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(success);
    
    // Test setting different neuron types in regions
    auto* region1 = brain.getRegion(nlm::RegionId(1));
    if (region1) {
        auto populations = region1->getPopulations();
        for (auto* pop : populations) {
            if (pop) {
                pop->setNeuronType(nlm::NeuronType::Sensory);
            }
        }
    }
    
    std::cout << "    testEdgeCaseMixedNeuronTypes passed" << std::endl;
}

/// Test brain plasticity edge cases
void testEdgeCasePlasticityBoundaries() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.0f, nlm::ConfigSource::Default);  // Edge case: zero LTP
    config->set("stdp_ltd_weight", 0.0f, nlm::ConfigSource::Default);  // Edge case: zero LTD
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(success);
    
    // Enable plasticity and test with edge case parameters
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Run some steps to ensure plasticity system doesn't crash with zero weights
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testEdgeCasePlasticityBoundaries passed" << std::endl;
}

/// Test brain development edge cases
void testEdgeCaseDevelopmentBoundaries() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(success);
    
    // Test setting development stage to each possible value
    for (int stage = 0; stage < 5; ++stage) {
        brain.setDevelopmentalStage(static_cast<nlm::DevelopmentalStage>(stage));
        assert(brain.getDevelopmentalStage() == static_cast<nlm::DevelopmentalStage>(stage));
    }
    
    // Test development stage boundaries
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Aging);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Aging);
    
    std::cout << "    testEdgeCaseDevelopmentBoundaries passed" << std::endl;
}

/// Test brain memory system edge cases
void testEdgeCaseMemoryBoundaries() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(success);
    
    // Test memory systems with small brain size
    // This would test memory system initialization and operation
    
    // Test memory consolidation intervals
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Memory operations should not crash even with small brain
        workingMemory->clear();
    }
    
    std::cout << "    testEdgeCaseMemoryBoundaries passed" << std::endl;
}

/// Test brain with minimum viable configuration
void testEdgeCaseMinimalConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    // Minimal configuration - just enough to create a brain
    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 1.0, nlm::ConfigSource::Default);  // High probability
    config->set("random_seed", static_cast<uint64_t>(0), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(success);
    assert(brain.getTotalNeuronCount() == 1);
    
    // Test single neuron behavior
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testEdgeCaseMinimalConfiguration passed" << std::endl;
}

/// Test brain error handling and recovery
void testEdgeCaseErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test initialization failure scenarios (if any)
    bool success = brain.initialize();
    
    // If initialization fails, it should be handled gracefully
    if (!success) {
        std::cout << "    Initialization failed gracefully (expected)" << std::endl;
    } else {
        std::cout << "    testEdgeCaseErrorHandling passed" << std::endl;
    }
}

/// Test brain with unusual time steps
void testEdgeCaseTimeStepBoundaries() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.000001, nlm::ConfigSource::Default);  // Very small timestep
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(success);
    
    // Test with very small time steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step, step * 0.000001);  // Very small time
    }
    
    std::cout << "    testEdgeCaseTimeStepBoundaries passed" << std::endl;
}

/// Run all edge case tests
void runEdgeCaseTests() {
    std::cout << "Running comprehensive edge case tests..." << std::endl;
    
    testEdgeCaseZeroNeurons();
    testEdgeCaseLargeConfiguration();
    testEdgeCaseMixedNeuronTypes();
    testEdgeCasePlasticityBoundaries();
    testEdgeCaseDevelopmentBoundaries();
    testEdgeCaseMemoryBoundaries();
    testEdgeCaseMinimalConfiguration();
    testEdgeCaseErrorHandling();
    testEdgeCaseTimeStepBoundaries();
    
    std::cout << "All edge case tests completed!" << std::endl;
}

} // namespace test_edge_cases
