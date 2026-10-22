// Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include "core/test/TestFramework.hpp"
#include <iostream>
#include <cmath>

namespace test_synapse {

void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    nlm::test::TestAssert::equal(1ULL, synapse.getId().value, "testSynapseCreation");
    nlm::test::TestAssert::equal(1ULL, synapse.getSourceNeuron().value, "testSynapseCreation");
    nlm::test::TestAssert::equal(2ULL, synapse.getDestinationNeuron().value, "testSynapseCreation");
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    nlm::test::TestAssert::equal(0.0f, synapse.getWeight(), "testSynapseWeight");
    
    synapse.setWeight(0.5f);
    nlm::test::TestAssert::equal(0.5f, synapse.getWeight(), "testSynapseWeight");
    
    synapse.addToWeight(0.2f);
    nlm::test::TestAssert::equal(0.7f, synapse.getWeight(), "testSynapseWeight");
    
    synapse.addToWeight(-0.3f);
    nlm::test::TestAssert::equal(0.4f, synapse.getWeight(), "testSynapseWeight");
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseWeightBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    nlm::test::TestAssert::lessThan(1.0f, synapse.getWeight() + 1e-6f, "testSynapseWeightBounds");  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    nlm::test::TestAssert::greaterThan(-1.0f, synapse.getWeight() - 1e-6f, "testSynapseWeightBounds");  // Should be clamped
    
    std::cout << "    testSynapseWeightBounds passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    nlm::test::TestAssert::equal(nlm::SynapseType::Excitatory, synapse.getType(), "testSynapseType");
    nlm::test::TestAssert::equal(true, synapse.isExcitatory(), "testSynapseType");
    nlm::test::TestAssert::equal(true, !synapse.isInhibitory(), "testSynapseType");
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    nlm::test::TestAssert::equal(nlm::SynapseType::Inhibitory, synapse.getType(), "testSynapseType");
    nlm::test::TestAssert::equal(true, synapse.isInhibitory(), "testSynapseType");
    nlm::test::TestAssert::equal(true, !synapse.isExcitatory(), "testSynapseType");
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    nlm::test::TestAssert::equal(1, synapse.getDelay(), "testSynapseDelay");  // Default
    
    synapse.setDelay(5);
    nlm::test::TestAssert::equal(5, synapse.getDelay(), "testSynapseDelay");
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseSpikeHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    nlm::test::TestAssert::equal(true, synapse.getPreSpikeHistory().empty(), "testSynapseSpikeHistory");
    nlm::test::TestAssert::equal(true, synapse.getPostSpikeHistory().empty(), "testSynapseSpikeHistory");
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    nlm::test::TestAssert::equal(2, synapse.getPreSpikeHistory().size(), "testSynapseSpikeHistory");
    nlm::test::TestAssert::equal(1, synapse.getPostSpikeHistory().size(), "testSynapseSpikeHistory");
    
    synapse.clearHistory();
    nlm::test::TestAssert::equal(true, synapse.getPreSpikeHistory().empty(), "testSynapseSpikeHistory");
    nlm::test::TestAssert::equal(true, synapse.getPostSpikeHistory().empty(), "testSynapseSpikeHistory");
    
    std::cout << "    testSynapseSpikeHistory passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    nlm::test::TestAssert::equal(0.0f, synapse.getEligibilityTrace(), "testSynapseEligibilityTrace");
    
    synapse.setEligibilityTrace(0.5f);
    nlm::test::TestAssert::equal(0.5f, synapse.getEligibilityTrace(), "testSynapseEligibilityTrace");
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    nlm::test::TestAssert::lessThan(0.5f, synapse.getEligibilityTrace() + 1e-6f, "testSynapseEligibilityTrace");
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    nlm::test::TestAssert::equal(1.0f, synapse.getEfficacy(), "testSynapseEfficacy");  // Default
    
    synapse.setEfficacy(0.5f);
    nlm::test::TestAssert::equal(0.5f, synapse.getEfficacy(), "testSynapseEfficacy");
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    nlm::test::TestAssert::lessThan(2.0f, synapse.getEfficacy() + 1e-6f, "testSynapseEfficacy");  // Clamped
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void testSynapseRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    nlm::test::TestAssert::greaterThan(synapse.getWeight(), 0.0f, "testSynapseRandomInitialization");
    
    // Delay should be in valid range
    nlm::test::TestAssert::greaterEqual(synapse.getDelay(), 1, "testSynapseRandomInitialization");
    nlm::test::TestAssert::lessEqual(synapse.getDelay(), 5, "testSynapseRandomInitialization");
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    nlm::test::TestAssert::equal(0.0f, synapse.getWeight(), "testSynapseReset");
    nlm::test::TestAssert::equal(true, synapse.getPreSpikeHistory().empty(), "testSynapseReset");
    nlm::test::TestAssert::equal(true, synapse.getPostSpikeHistory().empty(), "testSynapseReset");
    nlm::test::TestAssert::equal(0.0f, synapse.getEligibilityTrace(), "testSynapseReset");
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    nlm::test::TestAssert::equal(false, synapse.getPlasticityFlags().hebbian, "testSynapsePlasticityFlags");
    nlm::test::TestAssert::equal(false, synapse.getPlasticityFlags().stdp, "testSynapsePlasticityFlags");
    
    synapse.enablePlasticity(true, true, false);
    
    nlm::test::TestAssert::equal(true, synapse.getPlasticityFlags().hebbian, "testSynapsePlasticityFlags");
    nlm::test::TestAssert::equal(true, synapse.getPlasticityFlags().stdp, "testSynapsePlasticityFlags");
    nlm::test::TestAssert::equal(false, synapse.getPlasticityFlags().reward_modulated, "testSynapsePlasticityFlags");
    
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
