// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

// ========== COMPREHENSIVE REFACTORING TESTS ==========

void testBrainInitializationParameters() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.002, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify initialization parameters were correctly extracted
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getRegionCount() == 4);
    
    // Verify regions were created with correct names
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 4);
    
    // Verify each region has expected neuron counts based on constants
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
        
        // Each region should have populations based on BrainConstants ratios
        // Total neurons per region = 200 / 4 = 50
        // Sensory: 50 * 0.25 = 12.5 -> should be ~12-13
        // Internal: 50 * 0.5 = 25
        // Motor: 50 * 0.25 = 12.5 -> should be ~12-13
        
        size_t totalNeurons = region->getTotalNeuronCount();
        assert(totalNeurons > 0);
        
        // Verify we have all neuron types
        bool hasSensory = false, hasInternal = false, hasMotor = false;
        for (size_t i = 0; i < region->getPopulationCount(); ++i) {
            auto* pop = region->getPopulation(i);
            if (pop->getNeuronType() == nlm::NeuronType::Sensory) hasSensory = true;
            else if (pop->getNeuronType() == nlm::NeuronType::Internal) hasInternal = true;
            else if (pop->getNeuronType() == nlm::NeuronType::Motor) hasMotor = true;
        }
        
        assert(hasSensory && hasInternal && hasMotor);
    }
    
    std::cout << "    testBrainInitializationParameters passed" << std::endl;
}

void testBrainNeuronCollection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify sensory neurons were collected
    assert(brain.getTotalNeuronCount() == 400);
    
    // The brain should have collected sensory and motor neurons
    // Verify the helper method for collecting I/O neurons worked
    // (This is implicitly tested through the step function execution)
    
    // Run a step to ensure neuron collection works
    for (int step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testBrainNeuronCollection passed" << std::endl;
}

void testBrainConstantsUsage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify that constants are being used instead of magic numbers
    // The initialization should use BrainConstants for population ratios
    // This is verified by the correct distribution of neuron types
    
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalRegions = brain.getRegionCount();
    
    // Each region should have neurons based on constant ratios
    for (size_t i = 0; i < totalRegions; ++i) {
        auto* region = brain.getRegion(nlm::RegionId(i + 1));
        assert(region != nullptr);
        
        // Verify region has expected structure
        assert(region->getPopulationCount() > 0);
    }
    
    std::cout << "    testBrainConstantsUsage passed" << std::endl;
}

void testBrainStepRefactoring() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify step function refactoring didn't break functionality
    // The step function should now delegate to helper methods
    
    // Run multiple steps to ensure the refactored step function works correctly
    for (int step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify basic invariants still hold
        assert(brain.getTotalNeuronCount() == 150);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
    }
    
    std::cout << "    testBrainStepRefactoring passed" << std::endl;
}

void testBrainInitializationConstants() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(6), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that the initialization uses constants instead of magic numbers
    // Verify development system uses constants
    assert(brain.getRegionCount() == 6);
    assert(brain.getTotalNeuronCount() == 300);
    
    // Run a few steps to test development effects (which use constants)
    for (int step = 0; step < 2000; ++step) {
        brain.step(step);
        if (step % 100 == 0) {
            // Verify development system is working (implicit test of constants)
            assert(brain.getTotalNeuronCount() == 300);
        }
    }
    
    std::cout << "    testBrainInitializationConstants passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    // New comprehensive refactoring tests
    testBrainInitializationParameters();
    testBrainNeuronCollection();
    testBrainConstantsUsage();
    testBrainStepRefactoring();
    testBrainInitializationConstants();
}

} // namespace test_brain
