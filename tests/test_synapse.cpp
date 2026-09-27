// Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_synapse {

void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    if (synapse.getId() != nlm::SynapseId(1)) {
        throw nlm::NLMTestException("Synapse creation failed: ID should be 1, got " + 
                                   std::to_string(synapse.getId().getId()));
    }
    if (synapse.getSourceNeuron() != nlm::NeuronId(1)) {
        throw nlm::NLMTestException("Synapse creation failed: source neuron should be 1, got " + 
                                   std::to_string(synapse.getSourceNeuron().getId()));
    }
    if (synapse.getDestinationNeuron() != nlm::NeuronId(2)) {
        throw nlm::NLMTestException("Synapse creation failed: destination neuron should be 2, got " + 
                                   std::to_string(synapse.getDestinationNeuron().getId()));
    }
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    if (synapse.getWeight() != 0.0f) {
        throw nlm::NLMTestException("Synapse weight test failed: initial weight should be 0.0, got " + 
                                   std::to_string(synapse.getWeight()));
    }
    
    synapse.setWeight(0.5f);
    if (synapse.getWeight() != 0.5f) {
        throw nlm::NLMTestException("Synapse weight test failed: weight after set should be 0.5, got " + 
                                   std::to_string(synapse.getWeight()));
    }
    
    synapse.addToWeight(0.2f);
    if (synapse.getWeight() != 0.7f) {
        throw nlm::NLMTestException("Synapse weight test failed: weight after add should be 0.7, got " + 
                                   std::to_string(synapse.getWeight()));
    }
    
    synapse.addToWeight(-0.3f);
    if (synapse.getWeight() != 0.4f) {
        throw nlm::NLMTestException("Synapse weight test failed: weight after subtract should be 0.4, got " + 
                                   std::to_string(synapse.getWeight()));
    }
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseWeightBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    if (synapse.getWeight() > 1.0f) {
        throw nlm::NLMTestException("Synapse weight bounds test failed: weight should be <= 1.0 after setting 10.0, got " + 
                                   std::to_string(synapse.getWeight()));
    }
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    if (synapse.getWeight() < -1.0f) {
        throw nlm::NLMTestException("Synapse weight bounds test failed: weight should be >= -1.0 after setting -10.0, got " + 
                                   std::to_string(synapse.getWeight()));
    }
    
    std::cout << "    testSynapseWeightBounds passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    if (synapse.getType() != nlm::SynapseType::Excitatory) {
        throw nlm::NLMTestException("Synapse type test failed: type should be Excitatory after set, got " + 
                                   std::to_string(static_cast<int>(synapse.getType())));
    }
    if (!synapse.isExcitatory()) {
        throw nlm::NLMTestException("Synapse type test failed: should be excitatory");
    }
    if (synapse.isInhibitory()) {
        throw nlm::NLMTestException("Synapse type test failed: should not be inhibitory");
    }
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    if (synapse.getType() != nlm::SynapseType::Inhibitory) {
        throw nlm::NLMTestException("Synapse type test failed: type should be Inhibitory after set, got " + 
                                   std::to_string(static_cast<int>(synapse.getType())));
    }
    if (!synapse.isInhibitory()) {
        throw nlm::NLMTestException("Synapse type test failed: should be inhibitory");
    }
    if (synapse.isExcitatory()) {
        throw nlm::NLMTestException("Synapse type test failed: should not be excitatory");
    }
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    if (synapse.getDelay() != 1) {  // Default
        throw nlm::NLMTestException("Synapse delay test failed: default delay should be 1, got " + 
                                   std::to_string(synapse.getDelay()));
    }
    
    synapse.setDelay(5);
    if (synapse.getDelay() != 5) {
        throw nlm::NLMTestException("Synapse delay test failed: delay after set should be 5, got " + 
                                   std::to_string(synapse.getDelay()));
    }
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseSpikeHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    if (!synapse.getPreSpikeHistory().empty()) {
        throw nlm::NLMTestException("Synapse spike history test failed: pre spike history should be empty");
    }
    if (!synapse.getPostSpikeHistory().empty()) {
        throw nlm::NLMTestException("Synapse spike history test failed: post spike history should be empty");
    }
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    if (synapse.getPreSpikeHistory().size() != 2) {
        throw nlm::NLMTestException("Synapse spike history test failed: pre spike history size should be 2, got " + 
                                   std::to_string(synapse.getPreSpikeHistory().size()));
    }
    if (synapse.getPostSpikeHistory().size() != 1) {
        throw nlm::NLMTestException("Synapse spike history test failed: post spike history size should be 1, got " + 
                                   std::to_string(synapse.getPostSpikeHistory().size()));
    }
    
    synapse.clearHistory();
    if (!synapse.getPreSpikeHistory().empty()) {
        throw nlm::NLMTestException("Synapse spike history test failed: pre spike history should be empty after clear");
    }
    if (!synapse.getPostSpikeHistory().empty()) {
        throw nlm::NLMTestException("Synapse spike history test failed: post spike history should be empty after clear");
    }
    
    std::cout << "    testSynapseSpikeHistory passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    if (synapse.getEligibilityTrace() != 0.0f) {
        throw nlm::NLMTestException("Synapse eligibility trace test failed: initial trace should be 0.0, got " + 
                                   std::to_string(synapse.getEligibilityTrace()));
    }
    
    synapse.setEligibilityTrace(0.5f);
    if (synapse.getEligibilityTrace() != 0.5f) {
        throw nlm::NLMTestException("Synapse eligibility trace test failed: trace after set should be 0.5, got " + 
                                   std::to_string(synapse.getEligibilityTrace()));
    }
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    if (synapse.getEligibilityTrace() >= 0.5f) {
        throw nlm::NLMTestException("Synapse eligibility trace test failed: trace should be less than 0.5 after decay, got " + 
                                   std::to_string(synapse.getEligibilityTrace()));
    }
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    if (synapse.getEfficacy() != 1.0f) {  // Default
        throw nlm::NLMTestException("Synapse efficacy test failed: default efficacy should be 1.0, got " + 
                                   std::to_string(synapse.getEfficacy()));
    }
    
    synapse.setEfficacy(0.5f);
    if (synapse.getEfficacy() != 0.5f) {
        throw nlm::NLMTestException("Synapse efficacy test failed: efficacy after set should be 0.5, got " + 
                                   std::to_string(synapse.getEfficacy()));
    }
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    if (synapse.getEfficacy() > 2.0f) {  // Clamped
        throw nlm::NLMTestException("Synapse efficacy test failed: efficacy should be <= 2.0 after setting 5.0, got " + 
                                   std::to_string(synapse.getEfficacy()));
    }
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void testSynapseRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    if (synapse.getWeight() <= 0.0f) {
        throw nlm::NLMTestException("Synapse random initialization failed: weight should be > 0.0 for excitatory synapse, got " + 
                                   std::to_string(synapse.getWeight()));
    }
    
    // Delay should be in valid range
    if (synapse.getDelay() < 1 || synapse.getDelay() > 5) {
        throw nlm::NLMTestException("Synapse random initialization failed: delay should be in [1, 5], got " + 
                                   std::to_string(synapse.getDelay()));
    }
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    if (synapse.getWeight() != 0.0f) {
        throw nlm::NLMTestException("Synapse reset test failed: weight should be 0.0 after reset, got " + 
                                   std::to_string(synapse.getWeight()));
    }
    if (!synapse.getPreSpikeHistory().empty()) {
        throw nlm::NLMTestException("Synapse reset test failed: pre spike history should be empty after reset");
    }
    if (!synapse.getPostSpikeHistory().empty()) {
        throw nlm::NLMTestException("Synapse reset test failed: post spike history should be empty after reset");
    }
    if (synapse.getEligibilityTrace() != 0.0f) {
        throw nlm::NLMTestException("Synapse reset test failed: eligibility trace should be 0.0 after reset, got " + 
                                   std::to_string(synapse.getEligibilityTrace()));
    }
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    if (synapse.getPlasticityFlags().hebbian) {
        throw nlm::NLMTestException("Synapse plasticity flags test failed: hebbian should be false initially");
    }
    if (synapse.getPlasticityFlags().stdp) {
        throw nlm::NLMTestException("Synapse plasticity flags test failed: stdp should be false initially");
    }
    
    synapse.enablePlasticity(true, true, false);
    
    if (!synapse.getPlasticityFlags().hebbian) {
        throw nlm::NLMTestException("Synapse plasticity flags test failed: hebbian should be true after enable");
    }
    if (!synapse.getPlasticityFlags().stdp) {
        throw nlm::NLMTestException("Synapse plasticity flags test failed: stdp should be true after enable");
    }
    if (synapse.getPlasticityFlags().reward_modulated) {
        throw nlm::NLMTestException("Synapse plasticity flags test failed: reward_modulated should be false after enable");
    }
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

void runAll() {
    std::cout << "Running Synapse tests..." << std::endl;
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
}

} // namespace test_synapse
