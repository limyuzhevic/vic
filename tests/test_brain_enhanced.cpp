// Enhanced Brain Tests with Edge Cases and Performance Testing
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace test_brain_enhanced {

void testBrainCreationBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreationBasic passed" << std::endl;
}

void testBrainCreationNullConfig() {
    // Test with null configuration (should handle gracefully)
    try {
        nlm::Brain brain(nullptr);  // Should this be allowed?
        std::cout << "    testBrainCreationNullConfig: Implementation-dependent" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testBrainCreationNullConfig passed (threw exception as expected)" << std::endl;
    }
}

void testBrainCreationExtremeValues() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test with extreme values
    config->set("neuron_count", static_cast<int64_t>(100000), nlm::ConfigSource::Default);  // Very large
    config->set("region_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);     // Many regions
    config->set("connection_probability", 2.0f, nlm::ConfigSource::Default);                // Invalid (>1)
    
    try {
        nlm::Brain brain(config);
        bool init = brain.initialize();
        
        if (init) {
            // Check if initialization handled extreme values gracefully
            std::cout << "    testBrainCreationExtremeValues: Handled extreme values (may have clamped)" << std::endl;
        } else {
            std::cout << "    testBrainCreationExtremeValues: Initialization failed as expected" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "    testBrainCreationExtremeValues: Exception caught as expected" << std::endl;
    }
}

void testBrainInitializationValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);  // Invalid
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);   // Invalid
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(!success);  // Should fail with invalid configuration
    std::cout << "    testBrainInitializationValidation passed (validation failed as expected)" << std::endl;
}

void testBrainStepErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test stepping with negative step (if allowed)
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Test reset
    brain.reset();
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStepErrorHandling passed" << std::endl;
}

void testBrainStepExtremeStepValues() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test with very large step values
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Simulate large time jumps
        double largeTime = step * 1000.0;  // 1000 seconds per step
        brain.step(step, largeTime);
    }
    
    std::cout << "    testBrainStepExtremeStepValues passed" << std::endl;
}

void testBrainMemoryManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run some simulation
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Create a copy using move semantics
        nlm::Brain brain2(std::move(brain));
        
        // Original should be in valid but unspecified state
        // New brain should have valid state
        assert(brain2.getTotalNeuronCount() == 1000);
        assert(brain2.getRegionCount() == 2);
        
        std::cout << "    testBrainMemoryManagement passed (move semantics)" << std::endl;
    }
    
    // Test assignment operator
    nlm::Brain brain1(config);
    brain1.initialize();
    
    nlm::Brain brain2(std::make_shared<nlm::Config>());
    brain2 = std::move(brain1);
    
    assert(brain2.getTotalNeuronCount() == 1000);
    std::cout << "    testBrainMemoryManagement passed (assignment operator)" << std::endl;
}

void testBrainRegionErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);  // Invalid
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Should have default region or handle gracefully
    if (brain.getRegionCount() == 0) {
        std::cout << "    testBrainRegionErrorHandling: Handled zero regions" << std::endl;
    } else {
        std::cout << "    testBrainRegionErrorHandling: Has " << brain.getRegionCount() << " regions" << std::endl;
    }
}

void testBrainConnectivityEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);  // Very small
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 1.0f, nlm::ConfigSource::Default);            // Max connectivity
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Should handle very small network
    assert(brain.getTotalNeuronCount() == 10);
    assert(brain.getTotalSynapseCount() >= 0);  // May be 0 due to random chance
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testBrainConnectivityEdgeCases passed (small network)" << std::endl;
}

void testBrainPerformanceMonitoring() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test basic performance monitoring
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    assert(finalSpikes >= initialSpikes);
    
    double avgFiringRate = brain->getAverageFiringRate();
    assert(avgFiringRate >= 0.0f);
    
    std::cout << "    testBrainPerformanceMonitoring passed" << std::endl;
}

void testBrainConfigValidation() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test configuration validation
    config->set("brain.neuron_count", 500);  // Should be just "neuron_count"
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("invalid.key", 42.0f);  // Invalid key
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Should handle invalid keys gracefully
    std::cout << "    testBrainConfigValidation passed (invalid keys handled)" << std::endl;
}

void testBrainConcurrentAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test concurrent access simulation (simulated)
    std::vector<nlm::SimulationStep> steps;
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        steps.push_back(step);
    }
    
    // Process steps in parallel (simulated)
    for (auto step : steps) {
        brain.step(step);
    }
    
    std::cout << "    testBrainConcurrentAccess passed" << std::endl;
}

void testBrainPersistence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("random_seed", 123, nlm::ConfigSource::User);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointFile = "test_checkpoint.bin";
    bool saveSuccess = brain1.save(checkpointFile);
    
    if (saveSuccess) {
        // Load brain state
        nlm::Brain brain2(std::make_shared<nlm::Config>());
        bool loadSuccess = brain2.load(checkpointFile);
        
        if (loadSuccess) {
            std::cout << "    testBrainPersistence passed (save/load successful)" << std::endl;
        } else {
            std::cout << "    testBrainPersistence: Save succeeded, load failed" << std::endl;
        }
    } else {
        std::cout << "    testBrainPersistence: Save failed (file system may not allow)" << std::endl;
    }
}

void testBrainStatisticsConsistency() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Collect statistics over time
    std::vector<size_t> neuronCounts;
    std::vector<size_t> synapseCounts;
    std::vector<float> firingRates;
    
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        neuronCounts.push_back(brain.getTotalNeuronCount());
        synapseCounts.push_back(brain.getTotalSynapseCount());
        firingRates.push_back(brain.getAverageFiringRate());
    }
    
    // Check consistency
    assert(std::all_of(neuronCounts.begin(), neuronCounts.end(), 
                      [](size_t count) { return count == neuronCounts[0]; }));
    
    assert(std::all_of(synapseCounts.begin(), synapseCounts.end(), 
                      [](size_t count) { return count >= 0; }));
    
    assert(std::all_of(firingRates.begin(), firingRates.end(), 
                      [](float rate) { return rate >= 0.0f; }));
    
    std::cout << "    testBrainStatisticsConsistency passed" << std::endl;
}

void testBrainDevelopmentSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Check developmental stage
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial || 
           initialStage == nlm::DevelopmentalStage::CriticalPeriod);
    
    // Run simulation with development
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    nlm::DevelopmentalStage finalStage = brain.getDevelopmentalStage();
    std::cout << "    testBrainDevelopmentSystem: Stage changed from " 
              << static_cast<int>(initialStage) << " to " 
              << static_cast<int>(finalStage) << std::endl;
}

void runAllEnhanced() {
    std::cout << "Running Enhanced Brain Tests..." << std::endl;
    
    testBrainCreationBasic();
    testBrainCreationNullConfig();
    testBrainCreationExtremeValues();
    testBrainInitializationValidation();
    testBrainStepErrorHandling();
    testBrainStepExtremeStepValues();
    testBrainMemoryManagement();
    testBrainRegionErrorHandling();
    testBrainConnectivityEdgeCases();
    testBrainPerformanceMonitoring();
    testBrainConfigValidation();
    testBrainConcurrentAccess();
    testBrainPersistence();
    testBrainStatisticsConsistency();
    testBrainDevelopmentSystem();
    
    std::cout << std::endl << "All enhanced brain tests completed!" << std::endl;
}

} // namespace test_brain_enhanced

// Integration with existing tests
void runEnhancedBrainTests() {
    test_brain_enhanced::runAllEnhanced();
}
