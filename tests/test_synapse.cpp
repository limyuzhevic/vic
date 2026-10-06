// Synapse Tests
// Enhanced with NLM Test Framework
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <string>

// Test macros for NLM Test Framework
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

namespace test_synapse {

void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    NLM_ASSERT_EQ(synapse.getId(), nlm::SynapseId(1));
    NLM_ASSERT_EQ(synapse.getSourceNeuron(), nlm::NeuronId(1));
    NLM_ASSERT_EQ(synapse.getDestinationNeuron(), nlm::NeuronId(2));
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    NLM_ASSERT_EQ(synapse.getWeight(), 0.0f);
    
    synapse.setWeight(0.5f);
    NLM_ASSERT_EQ(synapse.getWeight(), 0.5f);
    
    synapse.addToWeight(0.2f);
    NLM_ASSERT_EQ(synapse.getWeight(), 0.7f);
    
    synapse.addToWeight(-0.3f);
    NLM_ASSERT_EQ(synapse.getWeight(), 0.4f);
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseWeightBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    NLM_ASSERT_LE(synapse.getWeight(), 1.0f);  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    NLM_ASSERT_GE(synapse.getWeight(), -1.0f);  // Should be clamped
    
    std::cout << "    testSynapseWeightBounds passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    NLM_ASSERT_EQ(synapse.getType(), nlm::SynapseType::Excitatory);
    NLM_ASSERT_TRUE(synapse.isExcitatory());
    NLM_ASSERT_FALSE(synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    NLM_ASSERT_EQ(synapse.getType(), nlm::SynapseType::Inhibitory);
    NLM_ASSERT_TRUE(synapse.isInhibitory());
    NLM_ASSERT_FALSE(synapse.isExcitatory());
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    NLM_ASSERT_EQ(synapse.getDelay(), 1);  // Default
    
    synapse.setDelay(5);
    NLM_ASSERT_EQ(synapse.getDelay(), 5);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseSpikeHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    NLM_ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
    NLM_ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    NLM_ASSERT_EQ(synapse.getPreSpikeHistory().size(), 2);
    NLM_ASSERT_EQ(synapse.getPostSpikeHistory().size(), 1);
    
    synapse.clearHistory();
    NLM_ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
    NLM_ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeHistory passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    NLM_ASSERT_EQ(synapse.getEligibilityTrace(), 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    NLM_ASSERT_EQ(synapse.getEligibilityTrace(), 0.5f);
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    NLM_ASSERT_LT(synapse.getEligibilityTrace(), 0.5f);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    NLM_ASSERT_EQ(synapse.getEfficacy(), 1.0f);  // Default
    
    synapse.setEfficacy(0.5f);
    NLM_ASSERT_EQ(synapse.getEfficacy(), 0.5f);
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    NLM_ASSERT_LE(synapse.getEfficacy(), 2.0f);  // Clamped
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void testSynapseRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    NLM_ASSERT_GT(synapse.getWeight(), 0.0f);
    
    // Delay should be in valid range
    NLM_ASSERT_TRUE(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    NLM_ASSERT_EQ(synapse.getWeight(), 0.0f);
    NLM_ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
    NLM_ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
    NLM_ASSERT_EQ(synapse.getEligibilityTrace(), 0.0f);
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    NLM_ASSERT_FALSE(synapse.getPlasticityFlags().hebbian);
    NLM_ASSERT_FALSE(synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    NLM_ASSERT_TRUE(synapse.getPlasticityFlags().hebbian);
    NLM_ASSERT_TRUE(synapse.getPlasticityFlags().stdp);
    NLM_ASSERT_FALSE(synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

// Modern test with enhanced features using modern assertions
void testSynapseAdvancedFeatures() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test advanced features with modern assertions
    synapse.setType(nlm::SynapseType::Excitatory);
    NLM_ASSERT_TRUE(synapse.isExcitatory());
    NLM_ASSERT_FALSE(synapse.isInhibitory());
    
    synapse.setWeight(0.8f);
    NLM_ASSERT_TRUE(synapse.getWeight() > 0.0f && synapse.getWeight() < 1.0f);
    
    synapse.setDelay(10);
    NLM_ASSERT_TRUE(synapse.getDelay() >= 1);
    
    std::cout << "    testSynapseAdvancedFeatures passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Synapse tests..." << std::endl;
    
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
    testSynapseAdvancedFeatures();
}

} // namespace test_synapse
