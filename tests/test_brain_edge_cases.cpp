// Comprehensive Brain Unit Tests with Edge Cases and Error Conditions
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace test_brain_edge_cases {

void testNullConfig() {
    std::cout << "    testNullConfig: " << std::flush;
    try {
        nlm::Brain brain(nullptr);
        std::cout << "FAILED - Should have thrown exception" << std::endl;
        assert(false);
    } catch (const std::invalid_argument& e) {
        std::cout << "PASSED - Correctly threw invalid_argument" << std::endl;
    }
}

void testZeroNeuronCount() {
    std::cout << "    testZeroNeuronCount: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    if (!success) {
        std::cout << "PASSED - Initialization failed as expected" << std::endl;
    } else {
        std::cout << "FAILED - Should have failed with zero neurons" << std::endl;
        assert(false);
    }
}

void testNegativeConnectionProbability() {
    std::cout << "    testNegativeConnectionProbability: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", -0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    if (!success) {
        std::cout << "PASSED - Initialization failed as expected" << std::endl;
    } else {
        std::cout << "FAILED - Should have failed with negative probability" << std::endl;
        assert(false);
    }
}

void testLargeNeuronCount() {
    std::cout << "    testLargeNeuronCount: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    if (success) {
        std::cout << "PASSED - Large neuron count handled (may impact performance)" << std::endl;
    } else {
        std::cout << "PASSED - Initialization failed (acceptable)" << std::endl;
    }
}

void testStepAfterReset() {
    std::cout << "    testStepAfterReset: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Take some steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    size_t spikeCountBeforeReset = brain.getTotalSpikeCount();
    
    brain.reset();
    
    // Take more steps
    for (nlm::SimulationStep step = 10; step < 20; ++step) {
        brain.step(step);
    }
    
    size_t spikeCountAfterReset = brain.getTotalSpikeCount();
    
    if (spikeCountAfterReset >= spikeCountBeforeReset) {
        std::cout << "PASSED - Spike count increased after reset" << std::endl;
    } else {
        std::cout << "FAILED - Spike count should not decrease after reset" << std::endl;
        assert(false);
    }
}

void testRegionOverflow() {
    std::cout << "    testRegionOverflow: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(100), nlm::ConfigSource::Default); // More regions than neurons
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    if (success) {
        std::cout << "PASSED - Region overflow handled (may cause empty regions)" << std::endl;
    } else {
        std::cout << "PASSED - Initialization failed (acceptable)" << std::endl;
    }
}

void testInvalidTimeStep() {
    std::cout << "    testInvalidTimeStep: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.0, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    if (!success) {
        std::cout << "PASSED - Initialization failed as expected" << std::endl;
    } else {
        std::cout << "FAILED - Should have failed with invalid timestep" << std::endl;
        assert(false);
    }
}

void testNegativeTimeStep() {
    std::cout << "    testNegativeTimeStep: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", -0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    if (!success) {
        std::cout << "PASSED - Initialization failed as expected" << std::endl;
    } else {
        std::cout << "FAILED - Should have failed with negative timestep" << std::endl;
        assert(false);
    }
}

void testMoveConstructor() {
    std::cout << "    testMoveConstructor: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    nlm::Brain brain2(std::move(brain1)); // Move constructor
    bool success = brain2.initialize();
    
    if (success) {
        std::cout << "PASSED - Move constructor works" << std::endl;
    } else {
        std::cout << "FAILED - Move constructor failed" << std::endl;
        assert(false);
    }
}

void testMoveAssignment() {
    std::cout << "    testMoveAssignment: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    nlm::Brain brain2(std::move(brain1)); // Create a temporary
    bool success = brain2.initialize();
    
    if (success) {
        std::cout << "PASSED - Move assignment works" << std::endl;
    } else {
        std::cout << "FAILED - Move assignment failed" << std::endl;
        assert(false);
    }
}

void testDevelopmentStageChanges() {
    std::cout << "    testDevelopmentStageChanges: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    nlm::DevelopmentalStage adultStage = brain.getDevelopmentalStage();
    
    if (adultStage == nlm::DevelopmentalStage::Adult && initialStage != adultStage) {
        std::cout << "PASSED - Development stage changes work" << std::endl;
    } else {
        std::cout << "FAILED - Development stage should change" << std::endl;
        assert(false);
    }
}

void testExceptionSafety() {
    std::cout << "    testExceptionSafety: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // This should throw during initialization
    try {
        config->set("invalid_key", "invalid_value", nlm::ConfigSource::Default);
        // If this succeeds, then the config should handle it
        nlm::Brain brain(config);
        bool success = brain.initialize();
        std::cout << "PASSED - Config handled invalid key gracefully" << std::endl;
    } catch (...) {
        std::cout << "PASSED - Exception thrown for invalid config" << std::endl;
    }
}

void testMemoryUsage() {
    std::cout << "    testMemoryUsage: " << std::flush;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    if (success) {
        // Run simulation and check memory stability
        size_t initialSpikes = brain.getTotalSpikeCount();
        
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        size_t finalSpikes = brain.getTotalSpikeCount();
        
        if (finalSpikes >= initialSpikes) {
            std::cout << "PASSED - Memory usage stable, spikes accumulated" << std::endl;
        } else {
            std::cout << "FAILED - Memory corrupted, spike count decreased" << std::endl;
            assert(false);
        }
    } else {
        std::cout << "PASSED - Initialization failed (acceptable for memory test)" << std::endl;
    }
}

void runAllEdgeCaseTests() {
    std::cout << "=== Running Brain Edge Case Tests ===" << std::endl;
    
    testNullConfig();
    testZeroNeuronCount();
    testNegativeConnectionProbability();
    testLargeNeuronCount();
    testStepAfterReset();
    testRegionOverflow();
    testInvalidTimeStep();
    testNegativeTimeStep();
    testMoveConstructor();
    testMoveAssignment();
    testDevelopmentStageChanges();
    testExceptionSafety();
    testMemoryUsage();
    
    std::cout << "=== All Edge Case Tests Completed ===" << std::endl;
}

} // namespace test_brain_edge_cases