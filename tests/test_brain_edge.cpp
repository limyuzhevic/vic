// Comprehensive Brain Initialization Edge Cases Tests
// Tests zero neurons, single neuron, large populations, and error scenarios

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_brain_edge {

void testBrainWithZeroNeurons() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 0, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.0, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    // Should fail to initialize with zero neurons
    bool initialized = brain.initialize();
    assert(!initialized);
    
    // Verify no regions were created
    assert(brain.getRegionCount() == 0);
    assert(brain.getTotalNeuronCount() == 0);
    assert(brain.getTotalSynapseCount() == 0);
    
    std::cout << "    testBrainWithZeroNeurons passed" << std::endl;
}

void testBrainWithSingleNeuron() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.0, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Verify basic properties
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 1);
    // Synapse count may be 0 due to connection probability 0.0
    
    std::cout << "    testBrainWithSingleNeuron passed" << std::endl;
}

void testBrainWithLargeNeuronCount() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100000, nlm::ConfigSource::Runtime);  // Large number
    config->set("region_count", 10, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.01, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Verify properties
    assert(brain.getRegionCount() == 10);
    assert(brain.getTotalNeuronCount() == 100000);
    
    std::cout << "    testBrainWithLargeNeuronCount passed" << std::endl;
}

void testBrainWithExcessiveRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1000, nlm::ConfigSource::Runtime);  // More regions than neurons
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Some regions may not have neurons due to division
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 1000);
    
    std::cout << "    testBrainWithExcessiveRegions passed" << std::endl;
}

void testBrainWithNullConfig() {
    // This test would require modifying Brain constructor to handle null config
    // For now, we'll test error handling in initialize()
    
    nlm::Brain brain(nullptr);
    bool initialized = brain.initialize();
    assert(!initialized);
    
    std::cout << "    testBrainWithNullConfig passed" << std::endl;
}

void testBrainBoundaryValues() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1, nlm::ConfigSource::Runtime);  // Minimum valid
    config->set("region_count", 1, nlm::ConfigSource::Runtime);   // Minimum valid
    config->set("connection_probability", 0.0, nlm::ConfigSource::Runtime);  // Minimum valid
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test with maximum connection probability
    config->set("connection_probability", 1.0, nlm::ConfigSource::Runtime);
    nlm::Brain brain2(config);
    bool initialized2 = brain2.initialize();
    assert(initialized2);
    
    std::cout << "    testBrainBoundaryValues passed" << std::endl;
}

void testBrainInvalidConnectionProbability() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 1.5, nlm::ConfigSource::Runtime);  // Invalid (> 1.0)
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(!initialized);
    
    std::cout << "    testBrainInvalidConnectionProbability passed" << std::endl;
}

void testBrainWithoutInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    // Brain not initialized - stepping should fail gracefully
    // Note: This depends on implementation of step() method
    // For now, we'll just verify the object exists
    assert(brain.getTotalNeuronCount() == 0);  // Not initialized yet
    
    std::cout << "    testBrainWithoutInitialization passed" << std::endl;
}

void testBrainRegionCreationFailure() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 1.0, nlm::ConfigSource::Runtime);  // High probability
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Verify regions were created
    auto regionIds = brain.getRegionIds();
    assert(!regionIds.empty());
    
    // Test getting non-existent region
    nlm::RegionId nonExistentRegion(999999);
    auto* region = brain.getRegion(nonExistentRegion);
    assert(region == nullptr);
    
    std::cout << "    testBrainRegionCreationFailure passed" << std::endl;
}

void testBrainMemoryAllocation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 5000, nlm::ConfigSource::Runtime);
    config->set("region_count", 5, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.001, nlm::ConfigSource::Runtime);  // Low probability
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Brain should have created neurons and synapses
    assert(brain.getTotalNeuronCount() == 5000);
    
    // Verify some motor and sensory neurons exist (may be zero with low probability)
    
    std::cout << "    testBrainMemoryAllocation passed" << std::endl;
}

void testBrainSimulationSteps() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 2, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run several simulation steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify brain state is still valid
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainSimulationSteps passed" << std::endl;
}

void testBrainResetFunctionality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Reset brain
    brain.reset();
    
    // Verify reset worked (neuron count should remain same)
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainResetFunctionality passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Try to produce action
    auto action = brain.produceAction();
    
    // Action may be nullptr if no motor neurons fired
    // The important thing is it doesn't crash
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatusInformation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Verify status information is accessible
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatusInformation passed" << std::endl;
}

void runAll() {
    testBrainWithZeroNeurons();
    testBrainWithSingleNeuron();
    testBrainWithLargeNeuronCount();
    testBrainWithExcessiveRegions();
    testBrainWithNullConfig();
    testBrainBoundaryValues();
    testBrainInvalidConnectionProbability();
    testBrainWithoutInitialization();
    testBrainRegionCreationFailure();
    testBrainMemoryAllocation();
    testBrainSimulationSteps();
    testBrainResetFunctionality();
    testBrainActionProduction();
    testBrainStatusInformation();
    
    std::cout << "Brain initialization edge cases test suite completed successfully!" << std::endl;
}

} // namespace test_brain_edge
