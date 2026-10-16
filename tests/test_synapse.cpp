// Synapse Tests - GoogleTest Version
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include "GoogleTestHelper.hpp"
#include "TestMatchers.hpp"

// TEST SUITE
class SynapseTestSuite : public ::testing::Test {
protected:
    nlm::Synapse synapse;
    
    void SetUp() override {
        synapse = nlm::Synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    }
};

// SYNAPSE TESTS
TEST_F(SynapseTestSuite, SynapseCreation) {
    EXPECT_EQ(synapse.getId(), nlm::SynapseId(1));
    EXPECT_EQ(synapse.getSourceNeuron(), nlm::NeuronId(1));
    EXPECT_EQ(synapse.getDestinationNeuron(), nlm::NeuronId(2));
}

TEST_F(SynapseTestSuite, SynapseWeight) {
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.0f);
    
    synapse.setWeight(0.5f);
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.5f);
    
    synapse.addToWeight(0.2f);
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.7f);
    
    synapse.addToWeight(-0.3f);
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.4f);
}

TEST_F(SynapseTestSuite, SynapseWeightBounds) {
    synapse.setWeight(10.0f);
    EXPECT_LE(synapse.getWeight(), 1.0f);
    
    synapse.setWeight(-10.0f);
    EXPECT_GE(synapse.getWeight(), -1.0f);
}

TEST_F(SynapseTestSuite, SynapseType) {
    synapse.setType(nlm::SynapseType::Excitatory);
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Excitatory);
    EXPECT_TRUE(synapse.isExcitatory());
    EXPECT_FALSE(synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Inhibitory);
    EXPECT_TRUE(synapse.isInhibitory());
    EXPECT_FALSE(synapse.isExcitatory());
}

TEST_F(SynapseTestSuite, SynapseDelay) {
    EXPECT_EQ(synapse.getDelay(), 1);
    
    synapse.setDelay(5);
    EXPECT_EQ(synapse.getDelay(), 5);
}

TEST_F(SynapseTestSuite, SynapseSpikeHistory) {
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    EXPECT_EQ(synapse.getPreSpikeHistory().size(), 2);
    EXPECT_EQ(synapse.getPostSpikeHistory().size(), 1);
    
    synapse.clearHistory();
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
}

TEST_F(SynapseTestSuite, SynapseEligibilityTrace) {
    EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.5f);
    
    synapse.decayEligibilityTrace(0.1f);
    EXPECT_LT(synapse.getEligibilityTrace(), 0.5f);
}

TEST_F(SynapseTestSuite, SynapseEfficacy) {
    EXPECT_FLOAT_EQ(synapse.getEfficacy(), 1.0f);
    
    synapse.setEfficacy(0.5f);
    EXPECT_FLOAT_EQ(synapse.getEfficacy(), 0.5f);
    
    synapse.setEfficacy(5.0f);
    EXPECT_LE(synapse.getEfficacy(), 2.0f);
}

TEST_F(SynapseTestSuite, SynapseRandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    EXPECT_GT(synapse.getWeight(), 0.0f);
    EXPECT_GE(synapse.getDelay(), 1);
    EXPECT_LE(synapse.getDelay(), 5);
}

TEST_F(SynapseTestSuite, SynapseReset) {
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.0f);
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
    EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.0f);
}

TEST_F(SynapseTestSuite, SynapsePlasticityFlags) {
    EXPECT_FALSE(synapse.getPlasticityFlags().hebbian);
    EXPECT_FALSE(synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    EXPECT_TRUE(synapse.getPlasticityFlags().hebbian);
    EXPECT_TRUE(synapse.getPlasticityFlags().stdp);
    EXPECT_FALSE(synapse.getPlasticityFlags().reward_modulated);
}
