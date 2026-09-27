// Synapse tests for weight clamping and bounds checking
// Tests for the improved Synapse.cpp functionality

#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_synapse_improved {

void testSynapseSetWeightClamping() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound clamping
    synapse.setWeight(10.0f);  // Should be clamped to 1.0f
    assert(synapse.getWeight() <= 1.0f && synapse.getWeight() >= -1.0f);
    assert(synapse.getWeight() == 1.0f);
    
    // Test lower bound clamping
    synapse.setWeight(-10.0f);  // Should be clamped to -1.0f
    assert(synapse.getWeight() <= 1.0f && synapse.getWeight() >= -1.0f);
    assert(synapse.getWeight() == -1.0f);
    
    // Test valid weight
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    std::cout << "    testSynapseSetWeightClamping passed" << std::endl;
}

void testSynapseAddToWeightWithBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Start from 0
    assert(synapse.getWeight() == 0.0f);
    
    // Test adding weight that doesn't exceed bounds
    synapse.addToWeight(0.3f);
    assert(synapse.getWeight() == 0.3f);
    
    // Test adding weight that would exceed upper bound
    synapse.addToWeight(0.9f);  // 0.3 + 0.9 = 1.2, should clamp to 1.0
    assert(synapse.getWeight() <= 1.0f);
    assert(synapse.getWeight() == 1.0f);
    
    // Test adding negative weight that would exceed lower bound
    synapse.addToWeight(-1.5f);  // 1.0 - 1.5 = -0.5, should clamp to -1.0
    assert(synapse.getWeight() >= -1.0f);
    assert(synapse.getWeight() == -1.0f);
    
    std::cout << "    testSynapseAddToWeightWithBounds passed" << std::endl;
}

void testSynapseEfficacyClamping() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound clamping
    synapse.setEfficacy(5.0f);  // Should be clamped to 2.0f
    assert(synapse.getEfficacy() <= 2.0f && synapse.getEfficacy() >= 0.0f);
    assert(synapse.getEfficacy() == 2.0f);
    
    // Test lower bound clamping
    synapse.setEfficacy(-1.0f);  // Should be clamped to 0.0f
    assert(synapse.getEfficacy() >= 0.0f && synapse.getEfficacy() <= 2.0f);
    assert(synapse.getEfficacy() == 0.0f);
    
    // Test valid efficacy
    synapse.setEfficacy(1.5f);
    assert(synapse.getEfficacy() == 1.5f);
    
    std::cout << "    testSynapseEfficacyClamping passed" << std::endl;
}

void testSynapseRandomInitializationWithBounds() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Excitatory synapses should have positive weights
    assert(synapse.getWeight() > 0.0f);
    assert(synapse.getWeight() <= 0.4f);  // From implementation
    assert(synapse.getWeight() >= 0.1f);  // From implementation
    
    // Efficacy should be in valid range
    assert(synapse.getEfficacy() >= 0.8f && synapse.getEfficacy() <= 1.0f);
    
    std::cout << "    testSynapseRandomInitializationWithBounds passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Set some values
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    synapse.setEfficacy(1.5f);
    
    assert(synapse.getWeight() == 0.5f);
    assert(synapse.getPreSpikeHistory().size() == 1);
    assert(synapse.getEligibilityTrace() == 0.3f);
    assert(synapse.getEfficacy() == 1.5f);
    
    // Reset should bring all to default
    synapse.reset();
    
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getEligibilityTrace() == 0.0f);
    assert(synapse.getEfficacy() == 1.0f);
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void runAllImprovedTests() {
    std::cout << "Running improved Synapse tests..." << std::endl;
    testSynapseSetWeightClamping();
    testSynapseAddToWeightWithBounds();
    testSynapseEfficacyClamping();
    testSynapseRandomInitializationWithBounds();
    testSynapseReset();
    std::cout << "All improved Synapse tests passed!" << std::endl;
}

} // namespace test_synapse_improved