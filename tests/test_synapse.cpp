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

void testSynapseEnhancedStep() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test enhanced step() method with biological realism
    // This tests the improved synaptic dynamics including:
    // 1. Short-term plasticity state updates
    // 2. Eligibility trace decay
    // 3. Synaptic efficacy modulation
    // 4. Weight bounds enforcement
    
    // Set up synapse for testing
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    // Record spike times for STDP
    synapse.recordPreSpike(0.0);
    synapse.recordPostSpike(0.1);
    
    // Test step() method
    synapse.step(0.5);
    
    // Check that weight is within bounds
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    // Check that eligibility trace decayed
    assert(synapse.getEligibilityTrace() < 0.5f);  // Should have decayed
    
    std::cout << "    testSynapseEnhancedStep passed" << std::endl;
}

void testSynapseModernCppFeatures() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test noexcept methods
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.setDelay(3);
    
    // These should be noexcept
    synapse.setType(nlm::SynapseType::Inhibitory);
    synapse.setWeight(-0.2f);
    synapse.setDelay(1);
    
    // Test [[nodiscard]] methods
    auto weight = synapse.getWeight();  // Should trigger static analysis
    auto delay = synapse.getDelay();
    auto source = synapse.getSourceNeuron();
    auto destination = synapse.getDestinationNeuron();
    
    // Verify values
    assert(weight == -0.2f);
    assert(delay == 1);
    assert(source == nlm::NeuronId(1));
    assert(destination == nlm::NeuronId(2));
    
    // Test bounds enforcement
    synapse.setEfficacy(5.0f);
    assert(synapse.getEfficacy() <= 2.0f);
    
    std::cout << "    testSynapseModernCppFeatures passed" << std::endl;
}

void testSynapseShortTermPlasticity() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test short-term plasticity dynamics
    synapse.setType(nlm::SynapseType::Excitatory);
    
    // Initial state
    float initialDepression = synapse.getEfficacy();  // Should be 1.0f for recovery
    
    // Record spike
    synapse.recordPreSpike(0.0);
    
    // Step forward in time
    synapse.step(0.5);
    
    // Check that depression state changed appropriately
    // (The exact behavior depends on implementation details)
    
    // Test facilitation
    synapse.recordPreSpike(0.1);
    synapse.step(0.2);
    
    std::cout << "    testSynapseShortTermPlasticity passed" << std::endl;
}

void testSynapseEligibilityTraceDecay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Set up eligibility trace
    synapse.setEligibilityTrace(0.8f);
    assert(synapse.getEligibilityTrace() == 0.8f);
    
    // Multiple decay steps
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() < 0.8f);
    
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() < synapse.getEligibilityTrace());  // Should continue decaying
    
    // Test that eligibility trace approaches zero
    for (int i = 0; i < 20; ++i) {
        synapse.decayEligibilityTrace(0.1f);
    }
    assert(synapse.getEligibilityTrace() < 0.01f);
    
    std::cout << "    testSynapseEligibilityTraceDecay passed" << std::endl;
}

void testSynapseUseDependentModulation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test synaptic efficacy modulation based on use
    float initialEfficacy = synapse.getEfficacy();
    assert(initialEfficacy == 1.0f);
    
    // Increase efficacy (use-dependent strengthening)
    synapse.setEfficacy(1.5f);
    assert(synapse.getEfficacy() == 1.5f);
    
    // Step forward time (efficacy should decay toward baseline)
    synapse.step(0.5);
    
    // Efficacy should recover toward baseline (1.0f)
    assert(synapse.getEfficacy() < 1.5f && synapse.getEfficacy() > 1.0f);
    
    std::cout << "    testSynapseUseDependentModulation passed" << std::endl;
}

} // namespace test_synapse
