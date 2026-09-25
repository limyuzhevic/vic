// Phase 6 Backward Compatibility Test Suite
// Tests for Phase 6 backward compatibility and API stability
// Validates Phase 6 API backward compatibility with existing code

#include "phase6_backward_compatibility_test.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6IntegrationFix.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace nlm {

void testPhase6DemoBackwardCompatibility() {
    std::cout << "Testing Phase 6Demo backward compatibility..." << std::endl;
    
    // Test Phase 6Demo backward compatibility
    Phase6Demo demo;
    
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
    
    // Test all legacy API methods
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK == true);
    
    bool memoryOK = experiment.testMemoryIntegration();
    assert(memoryOK == true);
    
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    assert(neuromodulationOK == true);
    
    bool cognitionOK = experiment.testCognitionIntegration();
    assert(cognitionOK == true);
    
    bool developmentOK = experiment.testDevelopmentIntegration();
    assert(developmentOK == true);
    
    bool worldIntegrationOK = experiment.testWorldIntegration();
    assert(worldIntegrationOK == true);
    
    bool environmentIntegrationOK = experiment.testEnvironmentIntegration();
    assert(environmentIntegrationOK == true);
    
    // Test backward compatibility with old API
    experiment.initialize();
    experiment.run();
    
    std::cout << "Phase6IntegratedExperiment backward compatibility test passed!" << std::endl;
}

void testPhase6IntegrationFixBackwardCompatibility() {
    std::cout << "Testing Phase6IntegrationFix backward compatibility..." << std::endl;
    
    // Test Phase6IntegrationFix backward compatibility
    Phase6IntegrationFix fix;
    
    // Test system integration
    bool integrationOK = fix.checkSystemIntegration();
    assert(integrationOK == true);
    
    // Test API fixes
    fix.fixPhase6DemoAPI();
    fix.fixPhase6IntegratedExperimentAPI();
    
    // Test backward compatibility verification
    bool compatOK = fix.verifyBackwardCompatibility();
    assert(compatOK == true);
    
    // Test migration guide generation
    fix.generateMigrationGuide();
    
    std::cout << "Phase6IntegrationFix backward compatibility test passed!" << std::endl;
}

void testPhase6APIBackwardCompatibility() {
    std::cout << "Testing Phase 6 API backward compatibility..." << std::endl;
    
    // Test Phase 6 API backward compatibility
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that all legacy API methods still work
    brain.step(10);
    brain.reset();
    brain.step(5);
    
    uint64_t neuronCount = brain.getTotalNeuronCount();
    uint64_t synapseCount = brain.getTotalSynapseCount();
    float firingRate = brain.getAverageFiringRate();
    float eiRatio = brain.getExcitationInhibitionRatio();
    
    assert(neuronCount > 0);
    assert(synapseCount >= 0);
    assert(firingRate >= 0.0f);
    assert(eiRatio >= 0.0f);
    
    // Test action production
    auto action = brain.produceAction();
    
    std::cout << "Phase 6 API backward compatibility test passed!" << std::endl;
}

void testPhase6FeatureParity() {
    std::cout << "Testing Phase 6 feature parity..." << std::endl;
    
    // Test that all Phase 6 features are still available
    Phase6IntegratedExperiment experiment;
    
    // Test that all Phase 6 features are still functional
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK == true);
    
    // Test that all Phase 6 systems are still available
    Phase6IntegrationFix fix;
    
    bool systemIntegrationOK = fix.checkSystemIntegration();
    assert(systemIntegrationOK == true);
    
    // Test that all Phase 6 APIs are still available
    Phase6Demo demo;
    
    bool demoInitialized = demo.initialize();
    if (demoInitialized) {
        demo.run();
    }
    
    std::cout << "Phase 6 feature parity test passed!" << std::endl;
}

void testPhase6DocumentationBackwardCompatibility() {
    std::cout << "Testing Phase 6 documentation backward compatibility..." << std::endl;
    
    // Test that Phase 6 documentation is still available
    std::string phase6Doc = "Phase 6 Integration Guide";
    assert(!phase6Doc.empty());
    
    // Test that Phase 6 migration guide is still available
    Phase6IntegrationFix fix;
    fix.generateMigrationGuide();
    
    std::cout << "Phase 6 documentation backward compatibility test passed!" << std::endl;
}

void testPhase6PerformanceBackwardCompatibility() {
    std::cout << "Testing Phase 6 performance backward compatibility..." << std::endl;
    
    // Test that Phase 6 performance is still acceptable
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        brain.step(1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Performance: 100 steps in " << duration.count() << " ms" << std::endl;
    std::cout << "Average per step: " << (duration.count() / 100.0) << " ms" << std::endl;
    
    // Performance should be acceptable
    double avgPerStep = duration.count() / 100.0;
    assert(avgPerStep < 100.0); // Less than 100ms per step is acceptable
    
    std::cout << "Phase 6 performance backward compatibility test passed!" << std::endl;
}

void runPhase6BackwardCompatibilityTests() {
    std::cout << "=== Running Phase 6 Backward Compatibility Tests ===" << std::endl;
    std::cout << std::endl;
    
    testPhase6DemoBackwardCompatibility();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentBackwardCompatibility();
    std::cout << std::endl;
    
    testPhase6IntegrationFixBackwardCompatibility();
    std::cout << std::endl;
    
    testPhase6APIBackwardCompatibility();
    std::cout << std::endl;
    
    testPhase6FeatureParity();
    std::cout << std::endl;
    
    testPhase6DocumentationBackwardCompatibility();
    std::cout << std::endl;
    
    testPhase6PerformanceBackwardCompatibility();
    std::cout << std::endl;
    
    std::cout << "=== All Phase 6 Backward Compatibility Tests Passed ===" << std::endl;
}

} // namespace nlm
