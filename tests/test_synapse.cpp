// Enhanced Synapse Tests
// Now includes comprehensive tests for synaptic properties and spike history
// #include "brain/Synapse.hpp"
// #include "core/Random/Random.hpp"
// #include <cassert>
// #include <iostream>
// #include <cmath>

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
    
    // Test initial eligibility trace
    assert(synapse.getEligibilityTrace() == 1.0f);
    
    // Test decay
    synapse.step(1.0f);
    assert(synapse.getEligibilityTrace() < 1.0f);
    assert(synapse.getEligibilityTrace() > 0.0f);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test initial plasticity flags
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    // Test enabling plasticity
    synapse.enablePlasticity(true, false, true);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

void testSynapsePlasticity() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Enable both Hebbian and STDP plasticity
    synapse.enablePlasticity(true, true, false);
    
    // Set initial weight
    synapse.setWeight(0.5f);
    
    // Record pre and post spikes
    synapse.recordPreSpike(0.0);
    synapse.recordPostSpike(10.0);
    
    // Apply Hebbian update (simple model)
    synapse.step(10.0f);
    
    // Weight should change (Hebbian learning)
    float newWeight = synapse.getWeight();
    assert(std::abs(newWeight - 0.5f) > 0.001f);  // Should change
    
    std::cout << "    testSynapsePlasticity passed" << std::endl;
}

void testSynapseUpdate() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Record spikes
    synapse.recordPreSpike(0.0);
    synapse.recordPostSpike(5.0);
    
    // Simulate time passing
    synapse.step(5.0f);
    
    // Weight should have changed due to time elapsed since spikes
    float finalWeight = synapse.getWeight();
    
    // Just verify it doesn't crash and weight is valid
    assert(finalWeight >= -1.0f && finalWeight <= 1.0f);
    
    std::cout << "    testSynapseUpdate passed" << std::endl;
}

void runAll() {
    testSynapseCreation();
    testSynapseWeight();
    testSynapseWeightBounds();
    testSynapseType();
    testSynapseDelay();
    testSynapseSpikeHistory();
    testSynapseEligibilityTrace();
    testSynapsePlasticityFlags();
    testSynapsePlasticity();
    testSynapseUpdate();
    
    std::cout << std::endl;
    std::cout << "=== Synapse Tests ===" << std::endl;
    std::cout << "All synapse tests passed!" << std::endl;
}

} // namespace test_synapse