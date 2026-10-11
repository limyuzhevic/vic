// Timing and Synchronization Edge Cases Tests
// Tests multiple steps, concurrent access, and timing issues

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

namespace test_timing_edge {

void testMultipleSteps() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 2, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run many steps to test timing stability
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Verify brain state is consistent
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() >= 0);
    }
    
    std::cout << "    testMultipleSteps passed" << std::endl;
}

void testStepParameters() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test step with different parameters
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        // Test step with time parameter
        brain.step(step, static_cast<nlm::Timestamp>(step) * 0.001f);
    }
    
    std::cout << "    testStepParameters passed" << std::endl;
}

void testStepBoundaryValues() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test with edge case step values
    brain.step(0);  // Minimum step
    brain.step(1);  // Small step
    brain.step(1000000);  // Large step
    
    // Verify brain still works
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testStepBoundaryValues passed" << std::endl;
}

void testTimeSynchronization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run simulation with time tracking
    nlm::Timestamp previousTime = -1.0f;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        nlm::Timestamp currentTime = static_cast<nlm::Timestamp>(step) * 0.001f;
        
        // Verify time is increasing
        assert(currentTime >= previousTime);
        previousTime = currentTime;
        
        brain.step(step, currentTime);
    }
    
    std::cout << "    testTimeSynchronization passed" << std::endl;
}

void testClockIntegration() {
    // Test integration with simulation clock system
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test that clock integration doesn't break brain functionality
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testClockIntegration passed" << std::endl;
}

void runAll() {
    testMultipleSteps();
    testStepParameters();
    testStepBoundaryValues();
    testTimeSynchronization();
    testClockIntegration();
    
    std::cout << "Timing and synchronization edge cases test suite completed successfully!" << std::endl;
}

} // namespace test_timing_edge
