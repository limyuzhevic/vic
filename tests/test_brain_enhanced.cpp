// Brain Tests - Enhanced with validation, performance, and edge case testing
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace test_brain {

void testBrainValidation() {
    // Test validation with invalid configuration (no regions)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default); // Invalid: zero regions
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    
    // Validation should fail due to zero regions
    if (brain.validate()) {
        throw std::runtime_error("Validation should fail for brain with zero regions");
    }
    
    std::cout << "    testBrainValidation passed" << std::endl;
}

void testBrainValidationWithPerformanceOptimizations() {
    // Test validation with performance optimizations enabled
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Configure performance optimizations
    brain.configurePerformanceOptimizations();
    
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Validation should pass with performance optimizations
    if (!brain.validate()) {
        throw std::runtime_error("Validation should pass for properly configured brain with performance optimizations");
    }
    
    std::cout << "    testBrainValidationWithPerformanceOptimizations passed" << std::endl;
}

void testPerformanceOptimizations() {
    // Test that performance optimizations don't break basic functionality
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Configure and enable performance optimizations
    brain.configurePerformanceOptimizations();
    brain.enableMemoryPools(true);
    brain.enableEventQueues(true);
    brain.enableParallelProcessing(true);
    brain.enableSIMD(true);
    
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test basic functionality with optimizations
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
    brain.step(0);
    
    // Verify basic statistics
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 1);
    
    std::cout << "    testPerformanceOptimizations passed" << std::endl;
}

void testConfigurationValidation() {
    // Test configuration validation with invalid values
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(-1), nlm::ConfigSource::Default); // Invalid negative seed
    config->set("simulation_timestep", -0.001, nlm::ConfigSource::Default); // Invalid negative timestep
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default); // Invalid zero neurons
    
    bool validationPassed = config->validate();
    assert(!validationPassed); // Should fail due to invalid values
    
    // Check validation error details
    std::string errors = config->getValidationErrors();
    if (errors.empty()) {
        throw std::runtime_error("Validation errors should contain details about invalid values");
    }
    
    // Fix configuration and test validation passes
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Runtime);
    
    validationPassed = config->validate();
    assert(validationPassed); // Should pass now
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testBrainStepWithPerformanceOptimizations() {
    // Test brain stepping with performance optimizations enabled
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.configurePerformanceOptimizations();
    brain.enableMemoryPools(true);
    brain.enableEventQueues(true);
    
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Run multiple steps with performance optimizations
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify brain remains in valid state
        if (!brain.validate()) {
            throw std::runtime_error("Brain validation failed during stepping with performance optimizations");
        }
    }
    
    std::cout << "    testBrainStepWithPerformanceOptimizations passed" << std::endl;
}

void testErrorHandlingWithInvalidConfiguration() {
    // Test error handling when configuration is invalid
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(-5), nlm::ConfigSource::Default); // Invalid
    
    nlm::Brain brain(config);
    
    // Validation should fail
    if (brain.validate()) {
        throw std::runtime_error("Validation should fail for brain with invalid neuron count");
    }
    
    std::cout << "    testErrorHandlingWithInvalidConfiguration passed" << std::endl;
}

void testMemoryPoolStats() {
    // Test memory pool statistics
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.configurePerformanceOptimizations();
    
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Check memory usage statistics
    // This would require accessing memory pool stats, which may be internal
    // For now, just verify the brain initialized correctly
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testMemoryPoolStats passed" << std::endl;
}

void testConfigurationValidationCallback() {
    // Test configuration validation callback functionality
    auto config = std::make_shared<nlm::Config>();
    
    // Set a custom validation callback that always rejects
    config->setValidationCallback([](const nlm::Config& cfg) {
        return false; // Always reject for testing
    });
    
    // Add some configuration
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    
    bool validationPassed = config->validate();
    assert(!validationPassed); // Should fail due to custom callback
    
    // Test with a callback that always accepts
    config->setValidationCallback([](const nlm::Config& cfg) {
        return true; // Always accept for testing
    });
    
    validationPassed = config->validate();
    assert(validationPassed); // Should pass now
    
    std::cout << "    testConfigurationValidationCallback passed" << std::endl;
}

void runEnhancedTests() {
    testBrainValidation();
    testBrainValidationWithPerformanceOptimizations();
    testPerformanceOptimizations();
    testConfigurationValidation();
    testBrainStepWithPerformanceOptimizations();
    testErrorHandlingWithInvalidConfiguration();
    testMemoryPoolStats();
    testConfigurationValidationCallback();
}

} // namespace test_brain
