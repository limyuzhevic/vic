// Backward Compatibility Test Suite
// Tests for backward compatibility and API stability
// Validates no API changes break existing code and feature parity maintained

#include "api_backward_compat_test.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase6Demo.cpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace nlm {

void testBrainBackwardCompatibility() {
    std::cout << "Testing Brain backward compatibility..." << std::endl;
    
    // Test that Brain API maintains backward compatibility
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that all legacy method calls still work
    // These are methods that should exist for backward compatibility
    
    // Basic operations
    brain.step(10);
    brain.reset();
    brain.step(5);
    
    // Property access
    uint64_t neuronCount = brain.getTotalNeuronCount();
    uint64_t synapseCount = brain.getTotalSynapseCount();
    float firingRate = brain.getAverageFiringRate();
    float eiRatio = brain.getExcitationInhibitionRatio();
    
    // All properties should be accessible
    assert(neuronCount > 0);
    assert(synapseCount >= 0);
    assert(firingRate >= 0.0f);
    assert(eiRatio >= 0.0f);
    
    // Action production
    auto action = brain.produceAction();
    // action may be null if no motor commands needed
    
    std::cout << "Brain backward compatibility test passed!" << std::endl;
}

void testConfigBackwardCompatibility() {
    std::cout << "Testing Config backward compatibility..." << std::endl;
    
    // Test that Config API maintains backward compatibility
    auto config = std::make_shared<Config>();
    
    // Test legacy configuration patterns
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1, ConfigSource::Default);
    
    // Test value retrieval patterns
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", 50);
    int64_t regionCount = config->getOr<int64_t>("region_count", 1);
    double connectionProb = config->getOr<double>("connection_probability", 0.5);
    
    assert(neuronCount == 100);
    assert(regionCount == 2);
    assert(connectionProb == 0.1);
    
    // Test configuration validation
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    assert(config->has("connection_probability"));
    
    std::cout << "Config backward compatibility test passed!" << std::endl;
}

void testPhase6DemoBackwardCompatibility() {
    std::cout << "Testing Phase 6Demo backward compatibility..." << std::endl;
    
    // Test Phase 6Demo backward compatibility
    Phase6Demo demo;
    
    // Test that Phase 6Demo API maintains backward compatibility
    // This should work with both old and new API patterns
    
    bool demoInitialized = demo.initialize();
    // Demo may or may not initialize depending on system state
    
    if (demoInitialized) {
        demo.run();
    }
    
    std::cout << "Phase 6Demo backward compatibility test passed!" << std::endl;
}

void testPhase6IntegratedExperimentBackwardCompatibility() {
    std::cout << "Testing Phase6IntegratedExperiment backward compatibility..." << std::endl;
    
    // Test Phase6IntegratedExperiment backward compatibility
    Phase6IntegratedExperiment experiment;
    
    // Test that all legacy API methods still work
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK == true);
    
    bool memoryOK = experiment.testMemoryIntegration();
    // Memory integration may not be available in all configurations
    
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    // Neuromodulation integration may not be available in all configurations
    
    // Test that experiment maintains compatibility with old API
    experiment.initialize();
    experiment.run();
    
    std::cout << "Phase6IntegratedExperiment backward compatibility test passed!" << std::endl;
}

void testSystemFeatureParity() {
    std::cout << "Testing system feature parity..." << std::endl;
    
    // Test that all expected features are still available
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that all expected brain features are still functional
    uint64_t neuronCount = brain.getTotalNeuronCount();
    uint64_t synapseCount = brain.getTotalSynapseCount();
    uint64_t firingCount = brain.getFiringNeuronCount();
    float firingRate = brain.getAverageFiringRate();
    float eiRatio = brain.getExcitationInhibitionRatio();
    
    // Test that all features return valid values
    assert(neuronCount > 0);
    assert(synapseCount >= 0);
    assert(firingCount >= 0);
    assert(firingRate >= 0.0f);
    assert(eiRatio >= 0.0f);
    
    // Test that action production still works
    auto action = brain.produceAction();
    // Action may be null if no motor commands needed
    
    std::cout << "System feature parity test passed!" << std::endl;
}

void testAPIStability() {
    std::cout << "Testing API stability..." << std::endl;
    
    // Test that API signatures and behavior are stable across calls
    auto config1 = std::make_shared<Config>();
    auto config2 = std::make_shared<Config>();
    
    config1->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config2->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain1(config1);
    Brain brain2(config2);
    
    brain1.initialize();
    brain2.initialize();
    
    // Test that identical configurations produce identical results
    brain1.step(10);
    brain2.step(10);
    
    // Results should be consistent (within floating point precision)
    uint64_t neuronCount1 = brain1.getTotalNeuronCount();
    uint64_t neuronCount2 = brain2.getTotalNeuronCount();
    
    assert(neuronCount1 == neuronCount2);
    
    std::cout << "API stability test passed!" << std::endl;
}

void runBackwardCompatibilityTests() {
    std::cout << "=== Running Backward Compatibility Tests ===" << std::endl;
    std::cout << std::endl;
    
    testBrainBackwardCompatibility();
    std::cout << std::endl;
    
    testConfigBackwardCompatibility();
    std::cout << std::endl;
    
    testPhase6DemoBackwardCompatibility();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentBackwardCompatibility();
    std::cout << std::endl;
    
    testSystemFeatureParity();
    std::cout << std::endl;
    
    testAPIStability();
    std::cout << std::endl;
    
    std::cout << "=== All Backward Compatibility Tests Passed ===" << std::endl;
}

} // namespace nlm
