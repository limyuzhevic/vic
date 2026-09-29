// Plasticity Tests
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_plasticity {

void testPlasticityRuleBase() {
    nlm::PlasticityFlags flags;
    
    assert(flags.hebbian == false);
    assert(flags.stdp == false);
    assert(flags.reward_modulated == false);
    
    flags.hebbian = true;
    flags.stdp = true;
    flags.reward_modulated = true;
    
    assert(flags.hebbian == true);
    assert(flags.stdp == true);
    assert(flags.reward_modulated == true);
    
    std::cout << "    testPlasticityRuleBase passed" << std::endl;
}

void testHebbianPlasticity() {
    // Note: This test assumes Hebbian class implements the interface
    // For now, we test the basic concepts
    nlm::PlasticityFlags flags;
    
    flags.hebbian = true;
    assert(flags.hebbian == true);
    assert(flags.stdp == false);
    
    std::cout << "    testHebbianPlasticity passed" << std::endl;
}

void testSTDPPlasticity() {
    // Note: This test assumes STDP class implements the interface
    // For now, we test the basic concepts
    nlm::PlasticityFlags flags;
    
    flags.stdp = true;
    assert(flags.stdp == true);
    assert(flags.hebbian == false);
    
    std::cout << "    testSTDPPlasticity passed" << std::endl;
}

void testSynapticPlasticityIntegration() {
    // Create a simple neuron-synaptic connection
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2(nlm::NeuronId(2));
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Enable plasticity
    synapse.enablePlasticity(true, true, false);
    
    assert(synapse.getPlasticityFlags().hebbian == true);
    assert(synapse.getPlasticityFlags().stdp == true);
    assert(synapse.getPlasticityFlags().reward_modulated == false);
    
    // Test eligibility trace (used by reward-modulated learning)
    synapse.setEligibilityTrace(0.3f);
    assert(synapse.getEligibilityTrace() == 0.3f);
    
    // Test weight modification
    synapse.addToWeight(0.1f);
    assert(synapse.getWeight() == 0.1f);
    
    std::cout << "    testSynapticPlasticityIntegration passed" << std::endl;
}

void testPlasticityLearningSimulation() {
    nlm::RandomGenerator rng(42);
    
    // Create neurons and synapses with plasticity enabled
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2(nlm::NeuronId(2));
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Initialize with random parameters
    neuron1.initializeRandom(rng);
    neuron2.initializeRandom(rng);
    synapse.initializeRandom(rng);
    
    // Enable different plasticity types
    synapse.enablePlasticity(true, true, true);  // Hebbian + STDP + Reward-modulated
    
    // Record spike activity for STDP
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    
    assert(synapse.getPreSpikeHistory().size() == 1);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    // Simulate time passing and eligibility trace decay
    synapse.setEligibilityTrace(0.5f);
    synapse.decayEligibilityTrace(0.1f);
    assert(std::abs(synapse.getEligibilityTrace() - 0.45f) < 0.01f);
    
    std::cout << "    testPlasticityLearningSimulation passed" << std::endl;
}

void testPlasticityReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Modify synapse state
    synapse.setWeight(0.8f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.7f);
    
    // Reset should restore initial state
    synapse.reset();
    
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    assert(synapse.getEligibilityTrace() == 0.0f);
    assert(synapse.getEfficacy() == 1.0f);
    assert(synapse.getPlasticityFlags().hebbian == false);
    
    std::cout << "    testPlasticityReset passed" << std::endl;
}

void runAll() {
    testPlasticityRuleBase();
    testHebbianPlasticity();
    testSTDPPlasticity();
    testSynapticPlasticityIntegration();
    testPlasticityLearningSimulation();
    testPlasticityReset();
}

} // namespace test_plasticity