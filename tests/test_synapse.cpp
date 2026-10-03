// Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_synapse {

void testSynapseCreation(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getId(nullptr) == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron(nullptr) == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron(nullptr) == nlm::NeuronId(2));
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getWeight(nullptr) == 0.0f);
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight(nullptr) == 0.5f);
    
    synapse.addToWeight(0.2f);
    assert(synapse.getWeight(nullptr) == 0.7f);
    
    synapse.addToWeight(-0.3f);
    assert(synapse.getWeight(nullptr) == 0.4f);
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseWeightBounds(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    assert(synapse.getWeight(nullptr) <= 1.0f);  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    assert(synapse.getWeight(nullptr) >= -1.0f);  // Should be clamped
    
    std::cout << "    testSynapseWeightBounds passed" << std::endl;
}

void testSynapseType(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.getType(nullptr) == nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory(nullptr));
    assert(!synapse.isInhibitory(nullptr));
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.getType(nullptr) == nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory(nullptr));
    assert(!synapse.isExcitatory(nullptr));
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseDelay(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getDelay(nullptr) == 1);  // Default
    
    synapse.setDelay(5);
    assert(synapse.getDelay(nullptr) == 5);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseSpikeHistory(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getPreSpikeHistory(nullptr).empty(nullptr));
    assert(synapse.getPostSpikeHistory(nullptr).empty(nullptr));
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    assert(synapse.getPreSpikeHistory(nullptr).size(nullptr) == 2);
    assert(synapse.getPostSpikeHistory(nullptr).size(nullptr) == 1);
    
    synapse.clearHistory(nullptr);
    assert(synapse.getPreSpikeHistory(nullptr).empty(nullptr));
    assert(synapse.getPostSpikeHistory(nullptr).empty(nullptr));
    
    std::cout << "    testSynapseSpikeHistory passed" << std::endl;
}

void testSynapseEligibilityTrace(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getEligibilityTrace(nullptr) == 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace(nullptr) == 0.5f);
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace(nullptr) < 0.5f);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getEfficacy(nullptr) == 1.0f);  // Default
    
    synapse.setEfficacy(0.5f);
    assert(synapse.getEfficacy(nullptr) == 0.5f);
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    assert(synapse.getEfficacy(nullptr) <= 2.0f);  // Clamped
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void testSynapseRandomInitialization(nullptr) {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    assert(synapse.getWeight(nullptr) > 0.0f);
    
    // Delay should be in valid range
    assert(synapse.getDelay(nullptr) >= 1 && synapse.getDelay(nullptr) <= 5);
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void testSynapseReset(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset(nullptr);
    
    assert(synapse.getWeight(nullptr) == 0.0f);
    assert(synapse.getPreSpikeHistory(nullptr).empty(nullptr));
    assert(synapse.getPostSpikeHistory(nullptr).empty(nullptr));
    assert(synapse.getEligibilityTrace(nullptr) == 0.0f);
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void testSynapsePlasticityFlags(nullptr) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(!synapse.getPlasticityFlags(nullptr).hebbian);
    assert(!synapse.getPlasticityFlags(nullptr).stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    assert(synapse.getPlasticityFlags(nullptr).hebbian);
    assert(synapse.getPlasticityFlags(nullptr).stdp);
    assert(!synapse.getPlasticityFlags(nullptr).reward_modulated);
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

void runAll(nullptr) {
    std::cout << "Running Synapse tests..." << std::endl;
    testSynapseCreation(nullptr);
    testSynapseWeight(nullptr);
    testSynapseWeightBounds(nullptr);
    testSynapseType(nullptr);
    testSynapseDelay(nullptr);
    testSynapseSpikeHistory(nullptr);
    testSynapseEligibilityTrace(nullptr);
    testSynapseEfficacy(nullptr);
    testSynapseRandomInitialization(nullptr);
    testSynapseReset(nullptr);
    testSynapsePlasticityFlags(nullptr);
}

} // namespace test_synapse
