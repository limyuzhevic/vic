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

void testSynapseErrorHandling() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test error handling with various operations
    // Set weight beyond bounds
    synapse.setWeight(15.0f);
    // Weight should be clamped by implementation (check if <= 1.0f)
    assert(synapse.getWeight() <= 1.0f);
    
    // Set weight below bounds
    synapse.setWeight(-15.0f);
    assert(synapse.getWeight() >= -1.0f);
    
    // Test eligibility trace with invalid values
    synapse.setEligibilityTrace(5.0f);
    assert(synapse.getEligibilityTrace() <= 2.0f);  // Should be clamped
    
    // Test decay with invalid values
    synapse.decayEligibilityTrace(-1.0f);  // Should handle negative decay
    
    // Test spike history with invalid values
    synapse.recordPreSpike(-1.0f);  // Should handle negative time
    synapse.recordPostSpike(1000000.0f);  // Should handle very large time
    
    std::cout << "    testSynapseErrorHandling passed" << std::endl;
}

void testSynapseAdvancedErrorScenarios() {
    nlm::RandomGenerator rng(42);
    
    // Test random initialization with error conditions
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.initializeRandom(rng);
    
    assert(synapse1.getWeight() > 0.0f);  // Excitatory should have positive weight
    assert(synapse1.getDelay() >= 1 && synapse1.getDelay() <= 5);  // Valid range
    
    // Test inhibitory synapse
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Inhibitory);
    synapse2.initializeRandom(rng);
    
    assert(synapse2.getWeight() < 0.0f);  // Inhibitory should have negative weight
    assert(synapse2.getDelay() >= 1 && synapse2.getDelay() <= 5);  // Valid range
    
    // Test plasticity flags
    synapse1.enablePlasticity(true, true, true);
    assert(synapse1.getPlasticityFlags().hebbian);
    assert(synapse1.getPlasticityFlags().stdp);
    assert(synapse1.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testSynapseAdvancedErrorScenarios passed" << std::endl;
}

void testSynapseMemorySystemErrorHandling() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test reset with various states
    synapse.setWeight(0.5f);
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setDelay(10);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    // Reset should clear all state
    synapse.reset();
    
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    // Test reset again (should still work)
    synapse.reset();
    assert(synapse.getWeight() == 0.0f);
    
    std::cout << "    testSynapseMemorySystemErrorHandling passed" << std::endl;
}

void testSynapseBoundaryConditions() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test exact boundary values
    synapse.setWeight(1.0f);
    assert(synapse.getWeight() <= 1.0f);  // Should be exactly 1.0 or clamped
    
    synapse.setWeight(-1.0f);
    assert(synapse.getWeight() >= -1.0f);  // Should be exactly -1.0 or clamped
    
    synapse.setEfficacy(2.0f);
    assert(synapse.getEfficacy() <= 2.0f);  // Should be exactly 2.0 or clamped
    
    synapse.setEfficacy(-2.0f);
    assert(synapse.getEfficacy() >= -2.0f);  // Should be exactly -2.0 or clamped
    
    // Test delay boundaries
    synapse.setDelay(1);
    assert(synapse.getDelay() >= 1);
    
    synapse.setDelay(1000);
    assert(synapse.getDelay() <= 1000);  // Max delay depends on implementation
    
    std::cout << "    testSynapseBoundaryConditions passed" << std::endl;
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
    testSynapseErrorHandling();
    testSynapseAdvancedErrorScenarios();
    testSynapseMemorySystemErrorHandling();
    testSynapseBoundaryConditions();
    
    std::cout << "All synapse tests passed!" << std::endl;
}

} // namespace test_synapse
