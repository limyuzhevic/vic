// Advanced Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_synapse_advanced {

void testSynapsePlasticityHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.enablePlasticity(true, true, false);
    
    // Record spike history
    synapse.recordPreSpike(0.0);
    synapse.recordPreSpike(1.0);
    synapse.recordPostSpike(0.5);
    
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    assert(synapse.getPreSpikeHistory()[0] == 0.0);
    assert(synapse.getPreSpikeHistory()[1] == 1.0);
    assert(synapse.getPostSpikeHistory()[0] == 0.5);
    
    std::cout << "    testSynapsePlasticityHistory passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Set eligibility trace
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    // Decay eligibility trace
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() < 0.5f);
    assert(synapse.getEligibilityTrace() > 0.0f);
    
    // Test trace decay to zero
    synapse.setEligibilityTrace(0.001f);
    synapse.decayEligibilityTrace(0.5f);  // Fast decay
    assert(std::abs(synapse.getEligibilityTrace()) < 0.001f);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseShortTermPlasticity() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Record pre and post spike times
    synapse.recordPreSpike(0.0);
    synapse.recordPostSpike(10.0);
    
    // Initial state
    assert(synapse.getEfficacy() >= 0.8f && synapse.getEfficacy() <= 1.0f);
    
    // Test efficacy bounds
    synapse.setEfficacy(2.5f);
    assert(synapse.getEfficacy() == 2.0f);  // Should be clamped to 2.0f max
    
    synapse.setEfficacy(-0.5f);
    assert(synapse.getEfficacy() == 0.0f);  // Should be clamped to 0.0f min
    
    std::cout << "    testSynapseShortTermPlasticity passed" << std::endl;
}

void testSynapseTypeAndWeightBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test initial type
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    // Change to inhibitory
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(!synapse.isExcitatory());
    assert(synapse.isInhibitory());
    
    // Test weight bounds
    synapse.setWeight(10.0f);
    assert(synapse.getWeight() <= 1.0f);  // Upper bound
    
    synapse.setWeight(-10.0f);
    assert(synapse.getWeight() >= -1.0f);  // Lower bound
    
    std::cout << "    testSynapseTypeAndWeightBounds passed" << std::endl;
}

void testSynapseRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Set type before initialization
    synapse.setType(nlm::SynapseType::Excitatory);
    
    // Initialize with random values
    synapse.initializeRandom(rng);
    
    // Check that weight is reasonable for excitatory synapse
    assert(synapse.getWeight() >= 0.1f && synapse.getWeight() <= 0.4f);
    assert(synapse.getEfficacy() >= 0.8f && synapse.getEfficacy() <= 1.0f);
    
    // Test inhibitory synapse initialization
    synapse.setType(nlm::SynapseType::Inhibitory);
    synapse.initializeRandom(rng);
    
    assert(synapse.getWeight() <= 0.0f && synapse.getWeight() >= -0.4f);
    assert(synapse.getEfficacy() >= 0.8f && synapse.getEfficacy() <= 1.0f);
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void runAll() {
    testSynapsePlasticityHistory();
    testSynapseEligibilityTrace();
    testSynapseShortTermPlasticity();
    testSynapseTypeAndWeightBounds();
    testSynapseRandomInitialization();
}

} // namespace test_synapse_advanced
