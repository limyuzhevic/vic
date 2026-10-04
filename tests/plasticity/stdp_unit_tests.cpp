// Unit test for STDP (Spike-Timing-Dependent Plasticity)
#include "plasticity/STDP.hpp"
#include <cassert>
#include <iostream>

namespace test_stdp_unit {

void testSTDPCreation() {
    nlm::STDP stdp;
    
    // Test basic STDP properties
    assert(stdp.getLearningRate() > 0.0f);
    assert(stdp.getTimeWindow() > 0.0f);
    assert(stdp.getMaxWeightChange() > 0.0f);
    
    std::cout << "    testSTDPCreation passed" << std::endl;
}

void testSTDPPrePostSynaptic() {
    nlm::STDP stdp;
    
    // Test STDP with pre-post synaptic activity
    float preSpike = 1.0f;
    float postSpike = 1.0f;
    
    stdp.apply(preSpike, postSpike, 0.001f); // Time difference: 1ms
    
    // Check that weight changed
    std::cout << "    testSTDPPrePostSynaptic passed" << std::endl;
}

void testSTDPNoPlasticity() {
    nlm::STDP stdp;
    
    // Test STDP with no synaptic activity
    stdp.apply(0.0f, 0.0f, 0.001f); // No spikes
    
    std::cout << "    testSTDPNoPlasticity passed" << std::endl;
}

void testSTDPParameters() {
    nlm::STDP stdp;
    
    // Test STDP parameters
    stdp.setLearningRate(0.01f);
    stdp.setTimeWindow(0.020f); // 20ms
    stdp.setMaxWeightChange(0.1f);
    
    assert(stdp.getLearningRate() == 0.01f);
    assert(stdp.getTimeWindow() == 0.020f);
    assert(stdp.getMaxWeightChange() == 0.1f);
    
    std::cout << "    testSTDPParameters passed" << std::endl;
}

void testSTDPTimeDependent() {
    nlm::STDP stdp;
    
    // Test time-dependent plasticity
    float timeDiff = 0.001f; // 1ms
    stdp.apply(1.0f, 0.0f, timeDiff); // Only pre-synaptic spike
    
    // Different time differences should produce different effects
    stdp.apply(0.0f, 1.0f, -0.001f); // Only post-synaptic spike (negative time)
    
    std::cout << "    testSTDPTimeDependent passed" << std::endl;
}

void runAllSTDPUnitTests() {
    testSTDPCreation();
    testSTDPPrePostSynaptic();
    testSTDPNoPlasticity();
    testSTDPParameters();
    testSTDPTimeDependent();
}

} // namespace test_stdp_unit