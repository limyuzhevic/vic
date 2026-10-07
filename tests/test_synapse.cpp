// Synapse Tests - Google Test Version
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <cmath>

namespace nlm {

class SynapseTest : public ::testing::Test {
protected:
    void SetUp() override {
        synapse = Synapse(SynapseId(1), NeuronId(1), NeuronId(2));
        rng = RandomGenerator(42);
    }
    
    Synapse synapse;
    RandomGenerator rng;
};

TEST_F(SynapseTest, SynapseCreation) {
    ASSERT_EQ(synapse.getId(), SynapseId(1));
    ASSERT_EQ(synapse.getSourceNeuron(), NeuronId(1));
    ASSERT_EQ(synapse.getDestinationNeuron(), NeuronId(2));
}

TEST_F(SynapseTest, SynapseWeight) {
    ASSERT_FLOAT_EQ(synapse.getWeight(), 0.0f);
    
    synapse.setWeight(0.5f);
    ASSERT_FLOAT_EQ(synapse.getWeight(), 0.5f);
    
    synapse.addToWeight(0.2f);
    ASSERT_FLOAT_EQ(synapse.getWeight(), 0.7f);
    
    synapse.addToWeight(-0.3f);
    ASSERT_FLOAT_EQ(synapse.getWeight(), 0.4f);
}

TEST_F(SynapseTest, SynapseWeightBounds) {
    // Test upper bound
    synapse.setWeight(10.0f);
    ASSERT_LE(synapse.getWeight(), 1.0f);  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    ASSERT_GE(synapse.getWeight(), -1.0f);  // Should be clamped
}

TEST_F(SynapseTest, SynapseType) {
    synapse.setType(SynapseType::Excitatory);
    ASSERT_EQ(synapse.getType(), SynapseType::Excitatory);
    ASSERT_TRUE(synapse.isExcitatory());
    ASSERT_FALSE(synapse.isInhibitory());
    
    synapse.setType(SynapseType::Inhibitory);
    ASSERT_EQ(synapse.getType(), SynapseType::Inhibitory);
    ASSERT_TRUE(synapse.isInhibitory());
    ASSERT_FALSE(synapse.isExcitatory());
    
    synapse.setType(SynapseType::Modulatory);
    ASSERT_EQ(synapse.getType(), SynapseType::Modulatory);
}

TEST_F(SynapseTest, SynapseDelay) {
    ASSERT_EQ(synapse.getDelay(), 1);  // Default
    
    synapse.setDelay(5);
    ASSERT_EQ(synapse.getDelay(), 5);
    
    // Test bounds
    synapse.setDelay(0);
    ASSERT_EQ(synapse.getDelay(), 1);  // Should be clamped to minimum
    
    synapse.setDelay(100);
    ASSERT_LE(synapse.getDelay(), 20);  // Should be clamped to maximum
}

TEST_F(SynapseTest, SynapseSpikeHistory) {
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

TEST_F(SynapseTest, SynapseHistoryLimits) {
    // Test that history is bounded
    for (int i = 0; i < 50; ++i) {
        synapse.recordPreSpike(static_cast<Timestamp>(i * 0.1));
        synapse.recordPostSpike(static_cast<Timestamp>(i * 0.1 + 0.05));
    }
    
    // History should be limited (maximum typically 20 entries)
    ASSERT_LE(synapse.getPreSpikeHistory().size(), 20);
    ASSERT_LE(synapse.getPostSpikeHistory().size(), 20);
}

TEST_F(SynapseTest, SynapseEligibilityTrace) {
    ASSERT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    ASSERT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.5f);
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    ASSERT_LT(synapse.getEligibilityTrace(), 0.5f);
    ASSERT_GT(synapse.getEligibilityTrace(), 0.0f);
    
    // Multiple decays
    synapse.setEligibilityTrace(1.0f);
    synapse.decayEligibilityTrace(0.3f);
    synapse.decayEligibilityTrace(0.3f);
    ASSERT_GT(synapse.getEligibilityTrace(), 0.0f);
}

TEST_F(SynapseTest, SynapseEfficacy) {
    ASSERT_FLOAT_EQ(synapse.getEfficacy(), 1.0f);  // Default
    
    synapse.setEfficacy(0.5f);
    ASSERT_FLOAT_EQ(synapse.getEfficacy(), 0.5f);
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    ASSERT_LE(synapse.getEfficacy(), 2.0f);  // Clamped
    
    synapse.setEfficacy(0.0f);
    ASSERT_GE(synapse.getEfficacy(), 0.1f);  // Minimum bound
}

TEST_F(SynapseTest, SynapseRandomInitialization) {
    synapse.setType(SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    ASSERT_GT(synapse.getWeight(), 0.0f);
    ASSERT_LT(synapse.getWeight(), 1.0f);
    
    // Delay should be in valid range
    ASSERT_GE(synapse.getDelay(), 1);
    ASSERT_LE(synapse.getDelay(), 5);
    
    // Efficacy should be positive
    ASSERT_GT(synapse.getEfficacy(), 0.0f);
}

TEST_F(SynapseTest, SynapseReset) {
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    ASSERT_FLOAT_EQ(synapse.getWeight(), 0.0f);
    ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
    ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
    ASSERT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.0f);
}

TEST_F(SynapseTest, SynapsePlasticityFlags) {
    ASSERT_FALSE(synapse.getPlasticityFlags().hebbian);
    ASSERT_FALSE(synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    ASSERT_TRUE(synapse.getPlasticityFlags().hebbian);
    ASSERT_TRUE(synapse.getPlasticityFlags().stdp);
    ASSERT_FALSE(synapse.getPlasticityFlags().reward_modulated);
    
    // Test plasticity with Synapses of different types
    synapse.setType(SynapseType::Excitatory);
    synapse.enablePlasticity(true, false, false);
    ASSERT_TRUE(synapse.getPlasticityFlags().hebbian);
    ASSERT_FALSE(synapse.getPlasticityFlags().stdp);
}

TEST_F(SynapseTest, SynapseIsDifferentTypes) {
    Synapse excSyn(SynapseId(2), NeuronId(3), NeuronId(4));
    Synapse inhSyn(SynapseId(3), NeuronId(5), NeuronId(6));
    
    excSyn.setType(SynapseType::Excitatory);
    inhSyn.setType(SynapseType::Inhibitory);
    
    ASSERT_TRUE(excSyn.isExcitatory());
    ASSERT_TRUE(inhSyn.isInhibitory());
    
    // Test synaptic weight changes don't affect type
    excSyn.addToWeight(0.5f);
    ASSERT_TRUE(excSyn.isExcitatory());
    
    inhSyn.addToWeight(-0.3f);
    ASSERT_TRUE(inhSyn.isInhibitory());
}

TEST_F(SynapseTest, SynapseDelayInfluence) {
    // Create two synapses with different delays
    Synapse fastSyn(SynapseId(4), NeuronId(7), NeuronId(8));
    Synapse slowSyn(SynapseId(5), NeuronId(9), NeuronId(10));
    
    fastSyn.setDelay(1);
    slowSyn.setDelay(10);
    
    // Delay should affect timing of synaptic effects
    // This is a conceptual test since timing is handled by simulation
    ASSERT_LT(fastSyn.getDelay(), slowSyn.getDelay());
}

TEST_F(SynapseTest, SynapseWithRandomSeeds) {
    // Test that different random seeds produce different results
    RandomGenerator seed1(12345);
    RandomGenerator seed2(54321);
    
    Synapse syn1(SynapseId(6), NeuronId(11), NeuronId(12));
    Synapse syn2(SynapseId(7), NeuronId(13), NeuronId(14));
    
    syn1.initializeRandom(seed1);
    syn2.initializeRandom(seed2);
    
    // Results should be different (low probability of collision)
    ASSERT_NE(syn1.getWeight(), syn2.getWeight());
}

} // namespace nlm

int main(int argc, char **argv) {
    std::cout << "=== NLM Synapse Tests (Google Test) ===" << std::endl;
    std::cout << "Testing Synaptic Properties and Behavior:" << std::endl;
    std::cout << "  - Synapse creation and identification" << std::endl;
    std::cout << "  - Weight management and bounds" << std::endl;
    std::cout << "  - Type classification (excitatory/inhibitory)" << std::endl;
    std::cout << "  - Transmission delays" << std::endl;
    std::cout << "  - Spike history recording" << std::endl;
    std::cout << "  - Eligibility trace dynamics" << std::endl;
    std::cout << "  - Random initialization" << std::endl;
    std::cout << "  - Reset behavior" << std::endl;
    std::cout << "  - Plasticity flag management" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
