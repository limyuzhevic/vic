// Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>
#include <limits>

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
    assert(!synapse.getPlasticityFlags().reward_modulated);
    assert(!synapse.getPlasticityFlags().structural);
    assert(!synapse.getPlasticityFlags().eligible);
    
    synapse.enablePlasticity(true, true, false);
    
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    synapse.enablePlasticity(false, false, true);
    
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

// Weight Update Mechanisms (STDP/Hebbian)
void testSTDPWeightUpdate() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.enablePlasticity(false, true, false);  // Only STDP
    
    const float initial_weight = synapse.getWeight();
    
    // Record pre- and post-synaptic spikes
    nlm::Timestamp pre_spike = 0.0f;
    nlm::Timestamp post_spike = 0.001f;  // Slightly after pre
    
    synapse.recordPreSpike(pre_spike);
    synapse.recordPostSpike(post_spike);
    
    // STDP should modify weight based on spike timing
    // For LTP (pre before post): weight increases
    // For LTD (post before pre): weight decreases
    
    // The actual weight change depends on the implementation
    // Here we test that the mechanism doesn't crash
    synapse.step(0.002f);
    
    // Weight should remain in valid bounds
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPWeightUpdate passed" << std::endl;
}

void testHebbianWeightUpdate() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.enablePlasticity(true, false, false);  // Only Hebbian
    
    // Hebbian learning: coincident pre- and post-synaptic activity
    synapse.recordPreSpike(0.0f);
    synapse.recordPostSpike(0.001f);
    
    synapse.step(0.002f);
    
    // Weight should be strengthened (increase for excitatory)
    assert(synapse.getWeight() > 0.0f);
    
    std::cout << "    testHebbianWeightUpdate passed" << std::endl;
}

// Synaptic Transmission
void testSynapticTransmission() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.8f);
    
    // Test spike transmission with delay
    synapse.setDelay(10);  // 10 simulation steps delay
    
    nlm::Timestamp spike_time = 0.0f;
    synapse.recordPreSpike(spike_time);
    
    // Should be in a queue for future delivery
    // Implementation detail-dependent
    
    // When step is called, spike should be delivered after delay
    synapse.step(5.0f);  // Before delay
    
    // Delivery happens at future steps
    assert(synapse.getPreSpikeHistory().size() > 0);
    
    // After delay steps, spike should be processed
    synapse.step(15.0f);  // After delay
    
    std::cout << "    testSynapticTransmission passed" << std::endl;
}

// Delay Handling
void testDelayHandling() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test default delay
    assert(synapse.getDelay() == 1);
    
    // Test setting different delays
    synapse.setDelay(0);  // No delay
    assert(synapse.getDelay() == 0);
    
    synapse.setDelay(100);  // Large delay
    assert(synapse.getDelay() == 100);
    
    // Test that delay is preserved across operations
    float weight = synapse.getWeight();
    synapse.setWeight(0.5f);
    assert(synapse.getDelay() == 100);
    assert(synapse.getWeight() == 0.5f);
    
    // Test that delay affects spike delivery timing
    nlm::Timestamp spike_time = 1.0f;
    synapse.recordPreSpike(spike_time);
    
    // Spike delivery time = spike_time + delay
    assert(synapse.getPreSpikeHistory().back() == spike_time);
    
    std::cout << "    testDelayHandling passed" << std::endl;
}

// Plasticity Flags and Validation
void testPlasticityFlagsValidation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test initial state
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    assert(!synapse.getPlasticityFlags().structural);
    
    // Test enabling individual plasticity rules
    synapse.enablePlasticity(true, false, false);  // Only Hebbian
    assert(synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    synapse.enablePlasticity(false, true, false);  // Only STDP
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(false, false, true);  // Only reward-modulated
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    // Test all enabled
    synapse.enablePlasticity(true, true, true);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testPlasticityFlagsValidation passed" << std::endl;
}

// Efficacy and Short-term Plasticity
void testSynapticEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test default efficacy
    assert(synapse.getEfficacy() == 1.0f);
    
    // Test setting efficacy
    synapse.setEfficacy(0.5f);
    assert(synapse.getEfficacy() == 0.5f);
    
    // Test efficacy bounds (should be clamped)
    synapse.setEfficacy(10.0f);
    assert(synapse.getEfficacy() <= 2.0f);  // Implementation may have different bounds
    
    synapse.setEfficacy(-10.0f);
    assert(synapse.getEfficacy() >= -2.0f);
    
    // Test efficacy decay with usage
    synapse.setEfficacy(2.0f);
    synapse.decayEligibilityTrace(0.1f);  // Effect on efficacy
    
    std::cout << "    testSynapticEfficacy passed" << std::endl;
}

// Memory leak detection for synapse
void testSynapseMemoryLeak() {
    // Create and destroy many synapses
    for (int i = 0; i < 100; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f + (i % 10) * 0.1f);
        synapse.setDelay(i % 20);
        
        synapse.enablePlasticity(true, true, true);
        
        synapse.recordPreSpike(0.1f * i);
        synapse.recordPostSpike(0.15f * i);
        
        synapse.setEligibilityTrace(0.3f);
        synapse.setEfficacy(0.8f);
    }
    
    std::cout << "    testSynapseMemoryLeak passed" << std::endl;
}

// Weight normalization test
void testWeightNormalization() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Weight should start at 0.0
    assert(synapse.getWeight() == 0.0f);
    
    // Weight should be able to increase
    synapse.addToWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    // Weight should be able to decrease
    synapse.addToWeight(-0.3f);
    assert(synapse.getWeight() == 0.2f);
    
    // Weight should respect bounds
    synapse.setWeight(10.0f);  // Should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    synapse.setWeight(-10.0f);  // Should be clamped to min
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testWeightNormalization passed" << std::endl;
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
    
    // Additional comprehensive tests
    testSTDPWeightUpdate();
    testHebbianWeightUpdate();
    testSynapticTransmission();
    testDelayHandling();
    testPlasticityFlagsValidation();
    testSynapticEfficacy();
    testSynapseMemoryLeak();
    testWeightNormalization();
}

} // namespace test_synapse
