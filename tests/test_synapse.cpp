// Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_synapse {

void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getWeight() == 0.0f);
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.2f);
    assert(synapse.getWeight() == 0.7f);
    
    synapse.addToWeight(-0.3f);
    assert(synapse.getWeight() == 0.4f);
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseWeightBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    assert(synapse.getWeight() <= 1.0f);  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    assert(synapse.getWeight() >= -1.0f);  // Should be clamped
    
    std::cout << "    testSynapseWeightBounds passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.getType() == nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    assert(!synapse.isExcitatory());
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getDelay() == 1);  // Default
    
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseSpikeHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeHistory passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() < 0.5f);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getEfficacy() == 1.0f);  // Default
    
    synapse.setEfficacy(0.5f);
    assert(synapse.getEfficacy() == 0.5f);
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    assert(synapse.getEfficacy() <= 2.0f);  // Clamped
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void testSynapseRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    assert(synapse.getWeight() > 0.0f);
    
    // Delay should be in valid range
    assert(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

// NEW COMPREHENSIVE TESTS

void testSynapseExtremeValues() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test extreme weight values
    synapse.setWeight(999.0f);
    assert(synapse.getWeight() <= 1.0f);  // Should be clamped to max
    
    synapse.setWeight(-999.0f);
    assert(synapse.getWeight() >= -1.0f);  // Should be clamped to min
    
    // Test extreme efficacy values
    synapse.setEfficacy(999.0f);
    assert(synapse.getEfficacy() <= 2.0f);  // Should be clamped
    
    // Test extreme delay values
    synapse.setDelay(0);  // Min delay
    assert(synapse.getDelay() == 0);
    
    synapse.setDelay(100);  // Large delay (not clamped in current implementation)
    assert(synapse.getDelay() == 100);
    
    std::cout << "    testSynapseExtremeValues passed" << std::endl;
}

void testSynapseStateConsistency() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Record initial spike history
    synapse.recordPreSpike(0.0);
    synapse.recordPostSpike(0.0);
    
    // Modify state
    synapse.setWeight(0.7f);
    synapse.setEfficacy(1.5f);
    synapse.setEligibilityTrace(0.3f);
    
    // Check consistency
    assert(synapse.getPreSpikeHistory().size() == 1);
    assert(synapse.getPostSpikeHistory().size() == 1);
    assert(synapse.getWeight() == 0.7f);
    assert(synapse.getEfficacy() == 1.5f);
    assert(synapse.getEligibilityTrace() == 0.3f);
    
    // Reset and check consistency
    synapse.reset();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getEfficacy() == 1.0f);  // Default
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    std::cout << "    testSynapseStateConsistency passed" << std::endl;
}

void testSynapseSpikeHistoryManagement() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Add many spikes (should be limited by MAX_SPIKE_HISTORY = 100)
    for (int i = 0; i < 150; ++i) {
        synapse.recordPreSpike(static_cast<float>(i) * 0.001f);
        synapse.recordPostSpike(static_cast<float>(i) * 0.001f);
    }
    
    // Should be clamped to max size
    assert(synapse.getPreSpikeHistory().size() <= 100);
    assert(synapse.getPostSpikeHistory().size() <= 100);
    
    // Test clear history
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeHistoryManagement passed" << std::endl;
}

void testSynapseEligibilityTraceDecay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Set eligibility trace
    synapse.setEligibilityTrace(0.8f);
    assert(synapse.getEligibilityTrace() == 0.8f);
    
    // Decay multiple times
    synapse.decayEligibilityTrace(0.1f);  // 10% decay
    float afterFirstDecay = synapse.getEligibilityTrace();
    assert(afterFirstDecay < 0.8f);
    
    synapse.decayEligibilityTrace(0.5f);  // 50% decay
    assert(synapse.getEligibilityTrace() < afterFirstDecay);
    
    // Set to zero and test decay doesn't go negative
    synapse.setEligibilityTrace(0.0f);
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    std::cout << "    testSynapseEligibilityTraceDecay passed" << std::endl;
}

void runAll() {
    testSynapseCreation();
    testSynapseWeight();
    testSynapseWeightBounds();
    testSynapseType();
    testSynapseDelay();
    testSynapseSpikeHistory();
    testSynapseEligibilityTrace();
    testSynapseEfficacy();
    testSynapseRandomInitialization();
    testSynapseReset();
    testSynapsePlasticityFlags();
    testSynapseExtremeValues();
    testSynapseStateConsistency();
    testSynapseSpikeHistoryManagement();
    testSynapseEligibilityTraceDecay();
}

} // namespace test_synapse
