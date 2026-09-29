// Synapse Tests
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_synapse {

void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(10));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(20));
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getWeight() == 0.0f);  // Initial weight
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.2f);
    assert(synapse.getWeight() == 0.7f);
    
    // Test weight bounds
    synapse.setWeight(2.0f);  // Should clamp to 1.0
    assert(synapse.getWeight() == 1.0f);
    
    synapse.setWeight(-2.0f);  // Should clamp to -1.0
    assert(synapse.getWeight() == -1.0f);
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getDelay() == 1);  // Default delay
    
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    assert(!synapse.isExcitatory());
    
    synapse.setType(nlm::SynapseType::Modulatory);
    assert(!synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseActivityHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    
    assert(synapse.getPreSpikeHistory().size() == 1);
    assert(synapse.getPostSpikeHistory().size() == 1);
    assert(synapse.getPreSpikeHistory()[0] == 0.1);
    assert(synapse.getPostSpikeHistory()[0] == 0.2);
    
    // Test history clearing
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseActivityHistory passed" << std::endl;
}

void testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getPlasticityFlags().hebbian == false);
    assert(synapse.getPlasticityFlags().stdp == false);
    assert(synapse.getPlasticityFlags().reward_modulated == false);
    
    synapse.enablePlasticity(true, false, true);
    assert(synapse.getPlasticityFlags().hebbian == true);
    assert(synapse.getPlasticityFlags().stdp == false);
    assert(synapse.getPlasticityFlags().reward_modulated == true);
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    // Test decay
    synapse.setEligibilityTrace(0.8f);
    synapse.decayEligibilityTrace(0.1f);
    assert(std::abs(synapse.getEligibilityTrace() - 0.72f) < 0.01f);
    
    // Test zero clamping
    synapse.setEligibilityTrace(0.001f);
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getEfficacy() == 1.0f);  // Initial efficacy
    
    synapse.setEfficacy(1.5f);
    assert(synapse.getEfficacy() == 1.5f);
    
    // Test efficacy bounds
    synapse.setEfficacy(3.0f);  // Should clamp to 2.0
    assert(synapse.getEfficacy() == 2.0f);
    
    synapse.setEfficacy(-0.5f);  // Should clamp to 0.0
    assert(synapse.getEfficacy() == 0.0f);
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void testSynapseRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    synapse.initializeRandom(rng);
    
    // Verify basic constraints
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    assert(synapse.getEfficacy() >= 0.0f && synapse.getEfficacy() <= 2.0f);
    assert(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    synapse.setWeight(0.8f);
    synapse.recordPreSpike(0.1);
    synapse.setEligibilityTrace(0.5f);
    
    synapse.reset();
    
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    assert(synapse.getEligibilityTrace() == 0.0f);
    assert(synapse.getEfficacy() == 1.0f);
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void runAll() {
    testSynapseCreation();
    testSynapseWeight();
    testSynapseDelay();
    testSynapseType();
    testSynapseActivityHistory();
    testSynapsePlasticityFlags();
    testSynapseEligibilityTrace();
    testSynapseEfficacy();
    testSynapseRandomInitialization();
    testSynapseReset();
}

} // namespace test_synapse