// Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include "nlm/test_framework.h"
#include <iostream>
#include <cmath>

namespace test_synapse {

TEST_CASE(testSynapseCreation) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_EQ(synapse.getId(), nlm::SynapseId(1));
    ASSERT_EQ(synapse.getSourceNeuron(), nlm::NeuronId(1));
    ASSERT_EQ(synapse.getDestinationNeuron(), nlm::NeuronId(2));
}

TEST_CASE(testSynapseWeight) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_EQ(synapse.getWeight(), 0.0f);
    
    synapse.setWeight(0.5f);
    ASSERT_EQ(synapse.getWeight(), 0.5f);
    
    synapse.addToWeight(0.2f);
    ASSERT_EQ(synapse.getWeight(), 0.7f);
    
    synapse.addToWeight(-0.3f);
    ASSERT_EQ(synapse.getWeight(), 0.4f);
}

TEST_CASE(testSynapseWeightBounds) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    ASSERT_LE(synapse.getWeight(), 1.0f);  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    ASSERT_GE(synapse.getWeight(), -1.0f);  // Should be clamped
}

TEST_CASE(testSynapseType) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    ASSERT_EQ(synapse.getType(), nlm::SynapseType::Excitatory);
    ASSERT_TRUE(synapse.isExcitatory());
    ASSERT_FALSE(synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    ASSERT_EQ(synapse.getType(), nlm::SynapseType::Inhibitory);
    ASSERT_TRUE(synapse.isInhibitory());
    ASSERT_FALSE(synapse.isExcitatory());
}

TEST_CASE(testSynapseDelay) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_EQ(synapse.getDelay(), 1);  // Default
    
    synapse.setDelay(5);
    ASSERT_EQ(synapse.getDelay(), 5);
}

TEST_CASE(testSynapseSpikeHistory) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
    ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    ASSERT_EQ(synapse.getPreSpikeHistory().size(), 2);
    ASSERT_EQ(synapse.getPostSpikeHistory().size(), 1);
    
    synapse.clearHistory();
    ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
    ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
}

TEST_CASE(testSynapseEligibilityTrace) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_EQ(synapse.getEligibilityTrace(), 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    ASSERT_EQ(synapse.getEligibilityTrace(), 0.5f);
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    ASSERT_LT(synapse.getEligibilityTrace(), 0.5f);
}

TEST_CASE(testSynapseEfficacy) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_EQ(synapse.getEfficacy(), 1.0f);  // Default
    
    synapse.setEfficacy(0.5f);
    ASSERT_EQ(synapse.getEfficacy(), 0.5f);
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    ASSERT_LE(synapse.getEfficacy(), 2.0f);  // Clamped
}

TEST_CASE(testSynapseRandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    ASSERT_TRUE(synapse.getWeight() > 0.0f);
    
    // Delay should be in valid range
    ASSERT_TRUE(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
}

TEST_CASE(testSynapseReset) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    ASSERT_EQ(synapse.getWeight(), 0.0f);
    ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
    ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
    ASSERT_EQ(synapse.getEligibilityTrace(), 0.0f);
}

TEST_CASE(testSynapsePlasticityFlags) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_FALSE(synapse.getPlasticityFlags().hebbian);
    ASSERT_FALSE(synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    ASSERT_TRUE(synapse.getPlasticityFlags().hebbian);
    ASSERT_TRUE(synapse.getPlasticityFlags().stdp);
    ASSERT_FALSE(synapse.getPlasticityFlags().reward_modulated);
}

std::vector<TestResult> runAllTests() {
    return test_synapse::runAllTests();
}

} // namespace test_synapse

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_synapse::runAllTests();
    printTestSummary(results);
}
