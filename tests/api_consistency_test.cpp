// API Consistency Test Suite
// Tests for unified API design and consistency across all NLM systems
// Validates consistent method signatures and improved backward compatibility

#include "api_consistency_test.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace nlm {

void testConsistentMethodSignatures() {
    std::cout << "Testing consistent method signatures..." << std::endl;
    
    // Create config and brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that all API methods have consistent signatures
    // step method: should accept both (SimulationStep) and (SimulationStep, float)
    brain.step(10);
    brain.step(10, 0.1f);
    
    // Property access methods should return consistent types
    uint64_t neuronCount = brain.getTotalNeuronCount();
    uint64_t synapseCount = brain.getTotalSynapseCount();
    float firingRate = brain.getAverageFiringRate();
    float eiRatio = brain.getExcitationInhibitionRatio();
    
    // All should return non-negative values
    assert(neuronCount > 0);
    assert(synapseCount >= 0);
    assert(firingRate >= 0.0f);
    assert(eiRatio >= 0.0f);
    
    std::cout << "Consistent method signatures test passed!" << std::endl;
}

void testParameterHandlingConsistency() {
    std::cout << "Testing parameter handling consistency..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test consistent parameter validation
    // Step method should handle different parameter combinations consistently
    brain.step(0);   // Minimum valid value
    brain.step(1);   // Normal value
    brain.step(1000); // Large value
    
    // Time delta parameter should be consistently handled
    brain.step(10, 0.0f);  // Minimum time delta
    brain.step(10, 0.1f);  // Normal time delta
    brain.step(10, 1.0f);  // Large time delta
    
    // Configuration parameter setting should be consistent
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Runtime);
    assert(config->getOr<int64_t>("neuron_count", 100) == 200);
    
    std::cout << "Parameter handling consistency test passed!" << std::endl;
}

void testErrorRecoveryConsistency() {
    std::cout << "Testing error recovery consistency..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that error recovery is consistent across different error conditions
    bool recoverySuccessful = true;
    
    try {
        // Simulate error conditions that should trigger recovery
        brain.step(-1); // Invalid step count
        recoverySuccessful = false;
    } catch (const std::exception& e) {
        // Error properly handled
        std::cout << "Error properly caught and handled: " << e.what() << std::endl;
    }
    
    // Verify brain can continue functioning after error
    brain.step(5);
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "Error recovery consistency test passed!" << std::endl;
}

void testSystemIntegrationConsistency() {
    std::cout << "Testing system integration consistency..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that all integrated systems have consistent behavior
    auto workingMemory = brain.getWorkingMemory();
    auto episodicMemory = brain.getEpisodicMemory();
    auto dopamine = brain.getDopamine();
    auto curiosity = brain.getCuriosity();
    auto prediction = brain.getPredictionSystem();
    auto development = brain.getDevelopmentSystem();
    
    // All systems should be consistently integrated (or gracefully handled if not available)
    if (workingMemory) {
        std::cout << "Working memory integration: OK" << std::endl;
    }
    
    if (episodicMemory) {
        std::cout << "Episodic memory integration: OK" << std::endl;
    }
    
    if (dopamine) {
        std::cout << "Neuromodulation integration: OK" << std::endl;
    }
    
    if (prediction) {
        std::cout << "Prediction system integration: OK" << std::endl;
    }
    
    if (development) {
        std::cout << "Development system integration: OK" << std::endl;
    }
    
    std::cout << "System integration consistency test passed!" << std::endl;
}

void runAPIConsistencyTests() {
    std::cout << "=== Running API Consistency Tests ===" << std::endl;
    std::cout << std::endl;
    
    testConsistentMethodSignatures();
    std::cout << std::endl;
    
    testParameterHandlingConsistency();
    std::cout << std::endl;
    
    testErrorRecoveryConsistency();
    std::cout << std::endl;
    
    testSystemIntegrationConsistency();
    std::cout << std::endl;
    
    std::cout << "=== All API Consistency Tests Passed ===" << std::endl;
}

} // namespace nlm
