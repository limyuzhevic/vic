// Test Hebbian Plasticity
#include "../../src/plasticity/Hebbian.hpp"
#include <cassert>
#include <iostream>

namespace test_hebbian {

void testHebbianPlasticity() {
    std::cout << "Testing Hebbian plasticity..." << std::endl;
    
    // Create Hebbian plasticity instance
    Hebbian plasticity;
    
    // Test initial values
    assert(plasticity.getLearningRate() > 0.0f);
    assert(plasticity.getEligibilityTraceDecay() > 0.0f);
    
    // Test plasticity factor calculation
    float factorBefore = plasticity.getPlasticityFactor();
    
    // Simulate pre-synaptic spike
    plasticity.recordPreSynapticSpike(1.0f);
    
    // Simulate post-synaptic spike
    plasticity.recordPostSynapticSpike(2.0f);
    
    // Update eligibility trace
    plasticity.updateEligibilityTrace(0.001f);
    
    // Calculate synaptic modification
    float delta = plasticity.calculateSynapticChange(1.0f);
    
    // Should produce potentiation (LTP) when pre fires before post
    assert(delta > 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    testHebbianPlasticity();
}

} // namespace test_hebbian