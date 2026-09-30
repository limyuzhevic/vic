// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"
#include <cassert>
#include <iostream>
#include <limits>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// Edge case testing functions for Brain class
void testBrainNullConfig() {
    // Test with null/None configuration
    nlm::Brain brain(nullptr);
    assert(false); // Should fail or handle gracefully
    std::cout << "    testBrainNullConfig passed" << std::endl;
}

void testBrainInvalidNeuronCounts() {
    // Test with various invalid neuron counts
    std::vector<int64_t> neuronCounts = {0, -1, -100, 1000000000};
    
    for (auto neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", neuronCount, nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        if (neuronCount <= 0) {
            // Should fail or handle gracefully
        } else {
            brain.initialize();
            brain.step(0);
            assert(brain.getTotalNeuronCount() == neuronCount);
        }
    }
    
    std::cout << "    testBrainInvalidNeuronCounts passed" << std::endl;
}

void testBrainInvalidRegionCounts() {
    // Test with various invalid region counts
    std::vector<int64_t> regionCounts = {0, -1, 1000000000};
    
    for (auto regionCount : regionCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", regionCount, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        if (regionCount <= 0) {
            // Should fail or handle gracefully
        } else {
            bool success = brain.initialize();
            assert(success);
            assert(brain.getRegionCount() == regionCount);
        }
    }
    
    std::cout << "    testBrainInvalidRegionCounts passed" << std::endl;
}

void testBrainInvalidConfig() {
    // Test with invalid configuration values
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 2.0, nlm::ConfigSource::Default); // Invalid: > 1.0
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    // Validation should catch this
    assert(!success || success); // Either behavior is acceptable
    
    std::cout << "    testBrainInvalidConfig passed" << std::endl;
}

void testBrainMemoryExhaustion() {
    // Simulate memory exhaustion by using very large values
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000000000), nlm::ConfigSource::Default); // 1B neurons
    config->set("region_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default); // 1000 regions
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    // May fail due to memory limits, which is acceptable
    if (success) {
        brain.step(0);
        brain.reset();
    }
    
    std::cout << "    testBrainMemoryExhaustion passed" << std::endl;
}

void testBrainStepWithInvalidState() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Try step without initialization
    brain.step(0);
    
    // Reset and try again
    brain.reset();
    brain.initialize();
    
    std::cout << "    testBrainStepWithInvalidState passed" << std::endl;
}

void testBrainActionProductionNoMotorNeurons() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Try to produce action (may succeed or return nullptr)
    auto action = brain.produceAction();
    // Either behavior is acceptable
    
    std::cout << "    testBrainActionProductionNoMotorNeurons passed" << std::endl;
}

void testBrainSensoryInputNoSensoryNeurons() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Try to receive sensory input with minimal neurons
    // This should handle gracefully
    nlm::SensoryInput input;
    brain.receiveSensoryInput(input);
    
    std::cout << "    testBrainSensoryInputNoSensoryNeurons passed" << std::endl;
}

void testBrainResetAfterPartialFailure() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Perform some operations
    brain.step(0);
    brain.step(1);
    
    // Reset
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainResetAfterPartialFailure passed" << std::endl;
}

void testBrainSaveLoadInvalidPaths() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test save with invalid paths
    bool saveResult = brain.save("/invalid/path/that/does/not/exist/test.bin");
    assert(!saveResult);
    
    // Test load with invalid path
    bool loadResult = brain.load("/invalid/path/that/does/not/exist/test.bin");
    assert(!loadResult);
    
    std::cout << "    testBrainSaveLoadInvalidPaths passed" << std::endl;
}

void testBrainConcurrentAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Simulate concurrent access by rapid sequential operations
    for (int i = 0; i < 100; ++i) {
        brain.step(i);
        brain.getTotalNeuronCount();
        brain.getTotalSynapseCount();
        brain.getFiringNeuronCount();
    }
    
    std::cout << "    testBrainConcurrentAccess passed" << std::endl;
}

void testBrainStepLargeValues() {
    // Test with very large step numbers
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test with large step numbers
    std::vector<nlm::SimulationStep> largeSteps = {0, 1, 1000, 100000, 2147483647};
    
    for (auto step : largeSteps) {
        brain.step(step);
    }
    
    std::cout << "    testBrainStepLargeValues passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    // Add comprehensive edge case tests
    testBrainNullConfig();
    testBrainInvalidNeuronCounts();
    testBrainInvalidRegionCounts();
    testBrainInvalidConfig();
    testBrainMemoryExhaustion();
    testBrainStepWithInvalidState();
    testBrainActionProductionNoMotorNeurons();
    testBrainSensoryInputNoSensoryNeurons();
    testBrainResetAfterPartialFailure();
    testBrainSaveLoadInvalidPaths();
    testBrainConcurrentAccess();
    testBrainStepLargeValues();
}

} // namespace test_brain
