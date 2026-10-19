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

void testSynapseValidation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test initial state validation
    assert(synapse.validateState());
    
    // Test weight validation
    synapse.setWeight(0.5f);  // Valid: -1.0 <= 0.5 <= 1.0
    assert(synapse.validateWeight(0.5f));
    
    synapse.setWeight(2.0f);   // Invalid: > 1.0
    assert(!synapse.validateWeight(2.0f));
    
    synapse.setWeight(-2.0f);  // Invalid: < -1.0
    assert(!synapse.validateWeight(-2.0f));
    
    // Test delay validation
    synapse.setDelay(5);   // Valid: 0 <= 5 <= 100
    assert(synapse.validateDelay(5));
    
    synapse.setDelay(-1);  // Invalid: < 0
    assert(!synapse.validateDelay(-1));
    
    synapse.setDelay(200); // Invalid: > 100
    assert(!synapse.validateDelay(200));
    
    // Test efficacy validation
    synapse.setEfficacy(1.0f);  // Valid: 0.0 <= 1.0 <= 2.0
    assert(synapse.validateEfficacy(1.0f));
    
    synapse.setEfficacy(3.0f);  // Invalid: > 2.0
    assert(!synapse.validateEfficacy(3.0f));
    
    synapse.setEfficacy(-1.0f); // Invalid: < 0.0
    assert(!synapse.validateEfficacy(-1.0f));
    
    // Test short-term depression validation
    synapse.setEfficacy(1.0f);  // Reset to valid
    float std = 0.5f;  // Valid: 0.0 <= 0.5 <= 1.0
    assert(synapse.validateShortTermDepression(std));
    
    float std_invalid = 1.5f;  // Invalid: > 1.0
    assert(!synapse.validateShortTermDepression(std_invalid));
    
    // Test short-term facilitation validation
    synapse.setEfficacy(1.0f);  // Reset to valid
    float stf = 1.0f;  // Valid: 0.0 <= 1.0 <= 2.0
    assert(synapse.validateShortTermFacilitation(stf));
    
    float stf_invalid = 2.5f;  // Invalid: > 2.0
    assert(!synapse.validateShortTermFacilitation(stf_invalid));
    
    // Test neuron ID validation
    assert(synapse.validateNeuronId(nlm::NeuronId(1)));
    assert(synapse.validateNeuronId(nlm::INVALID_NEURON_ID) == false);
    
    // Test synapse ID validation
    assert(synapse.validateSynapseId());
    assert(nlm::Synapse synapse2(nlm::INVALID_SYNAPSE_ID, nlm::NeuronId(1), nlm::NeuronId(2)));
    assert(synapse2.validateSynapseId() == false);
    
    // Test connection validation
    assert(synapse.validateConnection());
    
    nlm::Synapse synapse_self(nlm::SynapseId(3), nlm::NeuronId(1), nlm::NeuronId(1)); // Self-connection
    assert(synapse_self.validateConnection()); // Should pass validation but warn
    
    std::cout << "    testSynapseValidation passed" << std::endl;
}

void testSynapseValidationClamp() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test that setters clamp invalid values
    synapse.setWeight(2.0f);   // Should clamp to 1.0f
    assert(synapse.getWeight() == 1.0f);
    
    synapse.setWeight(-2.0f);  // Should clamp to -1.0f
    assert(synapse.getWeight() == -1.0f);
    
    synapse.setDelay(-1);      // Should clamp to 0
    assert(synapse.getDelay() == 0);
    
    synapse.setDelay(200);     // Should clamp to 100
    assert(synapse.getDelay() == 100);
    
    synapse.setEfficacy(3.0f);  // Should clamp to 2.0f
    assert(synapse.getEfficacy() == 2.0f);
    
    synapse.setEfficacy(-1.0f); // Should clamp to 0.0f
    assert(synapse.getEfficacy() == 0.0f);
    
    std::cout << "    testSynapseValidationClamp passed" << std::endl;
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
    testSynapseValidation();
    testSynapseValidationClamp();
    testSynapsePlasticityFlags();
}

} // namespace test_synapse
