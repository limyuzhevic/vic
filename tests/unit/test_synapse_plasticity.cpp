// Synapse Unit Tests using Google Test Framework
// Comprehensive unit tests for nlm::Synapse class
// Tests connection management, plasticity, and weight updates

#include "fixtures/TestHelpers.hpp"
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

namespace test {

// Test 5.1: Synapse Creation and Identity
TEST_F(NLMTest, SynapseCreation) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    EXPECT_EQ(synapse.getId(), nlm::SynapseId(1));
    EXPECT_EQ(synapse.getSourceNeuronId(), nlm::NeuronId(0));
    EXPECT_EQ(synapse.getTargetNeuronId(), nlm::NeuronId(1));
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Excitatory);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 1.0);
}

// Test 5.2: Synapse Type Management
TEST_F(NLMTest, SynapseTypeManagement) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Inhibitory);
    
    synapse.setType(nlm::SynapseType::Modulatory);
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Modulatory);
}

// Test 5.3: Synaptic Weight Management
TEST_F(NLMTest, SynapticWeightManagement) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    // Test initial weight
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 1.0);
    
    // Test setting weight
    synapse.setWeight(0.5);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 0.5);
    
    // Test weight increase
    synapse.increaseWeight(0.3);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 0.8);
    
    // Test weight decrease
    synapse.decreaseWeight(0.2);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 0.6);
    
    // Test weight clamping (weights shouldn't go below 0 or above some max)
    synapse.setWeight(-0.5);
    EXPECT_GE(synapse.getWeight(), 0.0);
    
    synapse.setWeight(2.0);
    EXPECT_LE(synapse.getWeight(), 1.0);  // Assuming max weight is 1.0
}

// Test 5.4: Delay Management
TEST_F(NLMTest, DelayManagement) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    EXPECT_DOUBLE_EQ(synapse.getDelay(), 0.0);
    
    synapse.setDelay(5.0);
    EXPECT_DOUBLE_EQ(synapse.getDelay(), 5.0);
    
    synapse.setDelay(0.1);
    EXPECT_DOUBLE_EQ(synapse.getDelay(), 0.1);
}

// Test 5.5: Strength Management
TEST_F(NLMTest, StrengthManagement) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    EXPECT_DOUBLE_EQ(synapse.getStrength(), 1.0);
    
    synapse.setStrength(2.5);
    EXPECT_DOUBLE_EQ(synapse.getStrength(), 2.5);
    
    synapse.setStrength(0.0);
    EXPECT_DOUBLE_EQ(synapse.getStrength(), 0.0);
}

// Test 5.6: State Management
TEST_F(NLMTest, SynapseStateManagement) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    EXPECT_FALSE(synapse.isActive());
    EXPECT_FALSE(synapse.isFailed());
    EXPECT_FALSE(synapse.isPlastic());
    
    synapse.setActive(true);
    EXPECT_TRUE(synapse.isActive());
    
    synapse.setFailed(true);
    EXPECT_TRUE(synapse.isFailed());
    
    synapse.setPlastic(true);
    EXPECT_TRUE(synapse.isPlastic());
}

// Test 5.7: Plasticity Management
TEST_F(NLMTest, PlasticityManagement) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    const auto& plasticityFlags = synapse.getPlasticityFlags();
    EXPECT_FALSE(plasticityFlags.hebbian);
    EXPECT_FALSE(plasticityFlags.stdp);
    EXPECT_FALSE(plasticityFlags.rewardModulated);
    
    synapse.enablePlasticity(true, false, true);
    
    const auto& updatedFlags = synapse.getPlasticityFlags();
    EXPECT_TRUE(updatedFlags.hebbian);
    EXPECT_FALSE(updatedFlags.stdp);
    EXPECT_TRUE(updatedFlags.rewardModulated);
}

// Test 5.8: Connection Properties
TEST_F(NLMTest, ConnectionProperties) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    EXPECT_EQ(synapse.getSourceNeuronId(), nlm::NeuronId(0));
    EXPECT_EQ(synapse.getTargetNeuronId(), nlm::NeuronId(1));
    
    // Test synapse with same source and target (self-connection)
    nlm::Synapse selfSynapse(nlm::SynapseId(2), nlm::NeuronId(0), nlm::NeuronId(0));
    EXPECT_EQ(selfSynapse.getSourceNeuronId(), selfSynapse.getTargetNeuronId());
}

// Test 5.9: Information Transmission
TEST_F(NLMTest, InformationTransmission) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    // Test spike arrival
    neuron::FiringState firingState = neuron::FiringState::Active;
    synapse.receiveSpike(firingState);
    
    EXPECT_TRUE(synapse.isActive());
    
    // Test neurotransmitter release
    double neurotransmitter = synapse.getNeurotransmitter();
    EXPECT_GE(neurotransmitter, 0.0);
    EXPECT_LE(neurotransmitter, 1.0);
}

// Test 5.10: Depression and Potentiation
TEST_F(NLMTest, DepressionAndPotentiation) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    // Record initial weight
    double initialWeight = synapse.getWeight();
    
    // Apply depression
    synapse.applyDepression(0.2);
    EXPECT_LT(synapse.getWeight(), initialWeight);
    
    // Reset for potentiation test
    synapse.setWeight(initialWeight);
    
    // Apply potentiation
    synapse.applyPotentiation(0.3);
    EXPECT_GT(synapse.getWeight(), initialWeight);
}

// Test 5.11: STDP Plasticity
TEST_F(NLMTest, STDPPlasticity) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    double initialWeight = synapse.getWeight();
    
    // Simulate STDP (pre-before-post)
    synapse.applySTDP(true, false);  // Pre before post
    EXPECT_GT(synapse.getWeight(), initialWeight);
    
    // Reset for LTD
    synapse.setWeight(initialWeight);
    
    // Simulate LTD (post-before-pre)
    synapse.applySTDP(false, true);  // Post before pre
    EXPECT_LT(synapse.getWeight(), initialWeight);
}

// Test 5.12: Synaptic Tagging and Marking
TEST_F(NLMTest, SynapticTagging) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    EXPECT_FALSE(synapse.hasTag(nlm::SynapticTag::LTD));
    EXPECT_FALSE(synapse.hasTag(nlm::SynapticTag::LTP));
    EXPECT_FALSE(synapse.hasTag(nlm::SynapticTag::DORMANCY));
    
    synapse.addTag(nlm::SynapticTag::LTD);
    EXPECT_TRUE(synapse.hasTag(nlm::SynapticTag::LTD));
    
    synapse.addTag(nlm::SynapticTag::LTP);
    EXPECT_TRUE(synapse.hasTag(nlm::SynapticTag::LTP));
    
    synapse.removeTag(nlm::SynapticTag::LTD);
    EXPECT_FALSE(synapse.hasTag(nlm::SynapticTag::LTD));
}

// Test 5.13: Synaptic History Management
TEST_F(NLMTest, SynapticHistory) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    EXPECT_TRUE(synapse.getRecentSpikes().empty());
    
    // Add spike history
    for (int i = 0; i < 5; ++i) {
        synapse.addRecentSpike(0.1 * i);
    }
    
    EXPECT_EQ(synapse.getRecentSpikes().size(), 5);
    EXPECT_DOUBLE_EQ(synapse.getRecentSpikes()[0], 0.0);
    EXPECT_DOUBLE_EQ(synapse.getRecentSpikes()[4], 0.4);
    
    synapse.clearRecentSpikes();
    EXPECT_TRUE(synapse.getRecentSpikes().empty());
}

// Test 5.14: Synaptic Noise
TEST_F(NLMTest, SynapticNoise) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    double originalWeight = synapse.getWeight();
    
    // Apply synaptic noise
    synapse.applyNoise(0.1);  // 10% noise level
    double noisyWeight = synapse.getWeight();
    
    // Weight should change due to noise
    EXPECT_NE(noisyWeight, originalWeight);
    
    // Noise should be within reasonable bounds
    double noiseRatio = std::abs(noisyWeight - originalWeight) / originalWeight;
    EXPECT_LE(noiseRatio, 0.1);  // Should be <= 10% noise
}

// Test 5.15: Synaptic Reliability
TEST_F(NLMTest, SynapticReliability) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    EXPECT_TRUE(synapse.isReliable());  // Default should be reliable
    
    synapse.setReliable(false);
    EXPECT_FALSE(synapse.isReliable());
    
    synapse.setReliable(true);
    EXPECT_TRUE(synapse.isReliable());
}

// Test 5.16: Synaptic Function
TEST_F(NLMTest, SynapticFunction) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    // Test basic synaptic function
    EXPECT_NO_THROW(synapse.step(0.001));  // Should not crash
    
    // Test signal transmission
    EXPECT_NO_THROW(synapse.transmitSignal());  // Should not crash
    
    // Test function with different parameters
    synapse.setWeight(0.5);
    synapse.setDelay(1.0);
    EXPECT_NO_THROW(synapse.step(0.001));
}

// Parameterized test for different synapse types
class SynapseTypeTest : public NLMTest, public ::testing::WithParamInterface<nlm::SynapseType> {
};

TEST_P(SynapseTypeTest, DifferentSynapseTypes) {
    nlm::SynapseType type = GetParam();
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    synapse.setType(type);
    
    EXPECT_EQ(synapse.getType(), type);
    
    // Test weight changes based on synapse type
    if (type == nlm::SynapseType::Excitatory) {
        EXPECT_DOUBLE_EQ(synapse.getWeight(), 1.0);
    } else if (type == nlm::SynapseType::Inhibitory) {
        EXPECT_LE(synapse.getWeight(), 0.0);  // Inhibitory weights are typically negative
    }
}

INSTANTIATE_TEST_SUITE_P(
    SynapseTypeVariations,
    SynapseTypeTest,
    ::testing::Values(
        nlm::SynapseType::Excitatory,
        nlm::SynapseType::Inhibitory,
        nlm::SynapseType::Modulatory
    )
);

// Test Synapse Plasticity Variations
TEST_F(NLMTest, SynapsePlasticityVariations) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    
    // Test different plasticity flag combinations
    synapse.enablePlasticity(true, true, false);  // hebbian + stdp
    auto flags1 = synapse.getPlasticityFlags();
    EXPECT_TRUE(flags1.hebbian);
    EXPECT_TRUE(flags1.stdp);
    EXPECT_FALSE(flags1.rewardModulated);
    
    synapse.enablePlasticity(false, false, true);  // reward only
    auto flags2 = synapse.getPlasticityFlags();
    EXPECT_FALSE(flags2.hebbian);
    EXPECT_FALSE(flags2.stdp);
    EXPECT_TRUE(flags2.rewardModulated);
}

} // namespace test