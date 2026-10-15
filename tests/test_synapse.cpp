// Synapse Tests - GoogleTest Version
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <cmath>

namespace {

TEST(SynapseTest, SynapseCreation) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    EXPECT_EQ(synapse.getId().index(), 1);
    EXPECT_EQ(synapse.getSourceNeuron().index(), 1);
    EXPECT_EQ(synapse.getDestinationNeuron().index(), 2);
}

TEST(SynapseTest, SynapseWeight) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 0.0);
    
    synapse.setWeight(0.5);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 0.5);
    
    synapse.addToWeight(0.2);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 0.7);
    
    synapse.addToWeight(-0.3);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 0.4);
}

TEST(SynapseTest, SynapseWeightBounds) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(10.0);
    EXPECT_LE(synapse.getWeight(), 1.0);
    
    synapse.setWeight(-10.0);
    EXPECT_GE(synapse.getWeight(), -1.0);
}

TEST(SynapseTest, SynapseType) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Excitatory);
    EXPECT_TRUE(synapse.isExcitatory());
    EXPECT_FALSE(synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Inhibitory);
    EXPECT_TRUE(synapse.isInhibitory());
    EXPECT_FALSE(synapse.isExcitatory());
}

TEST(SynapseTest, SynapseDelay) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    EXPECT_EQ(synapse.getDelay(), 1);
    
    synapse.setDelay(5);
    EXPECT_EQ(synapse.getDelay(), 5);
}

TEST(SynapseTest, SynapseSpikeHistory) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    auto preHistory = synapse.getPreSpikeHistory();
    auto postHistory = synapse.getPostSpikeHistory();
    
    EXPECT_EQ(preHistory.size(), 2);
    EXPECT_EQ(postHistory.size(), 1);
    EXPECT_DOUBLE_EQ(preHistory[0], 0.1);
    EXPECT_DOUBLE_EQ(preHistory[1], 0.2);
    EXPECT_DOUBLE_EQ(postHistory[0], 0.15);
    
    synapse.clearHistory();
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
}

TEST(SynapseTest, SynapseEligibilityTrace) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    EXPECT_DOUBLE_EQ(synapse.getEligibilityTrace(), 0.0);
    
    synapse.setEligibilityTrace(0.5);
    EXPECT_DOUBLE_EQ(synapse.getEligibilityTrace(), 0.5);
    
    synapse.decayEligibilityTrace(0.1);
    EXPECT_LT(synapse.getEligibilityTrace(), 0.5);
    EXPECT_GT(synapse.getEligibilityTrace(), 0.0);
}

TEST(SynapseTest, SynapseEfficacy) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    EXPECT_DOUBLE_EQ(synapse.getEfficacy(), 1.0);
    
    synapse.setEfficacy(0.5);
    EXPECT_DOUBLE_EQ(synapse.getEfficacy(), 0.5);
    
    synapse.setEfficacy(5.0);
    EXPECT_LE(synapse.getEfficacy(), 2.0);
}

TEST(SynapseTest, SynapseRandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    EXPECT_GT(synapse.getWeight(), 0.0);
    EXPECT_GE(synapse.getDelay(), 1);
    EXPECT_LE(synapse.getDelay(), 5);
}

TEST(SynapseTest, SynapseReset) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3);
    
    synapse.reset();
    
    EXPECT_DOUBLE_EQ(synapse.getWeight(), 0.0);
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
    EXPECT_DOUBLE_EQ(synapse.getEligibilityTrace(), 0.0);
}

TEST(SynapseTest, SynapsePlasticityFlags) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    EXPECT_FALSE(synapse.getPlasticityFlags().hebbian);
    EXPECT_FALSE(synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    EXPECT_TRUE(synapse.getPlasticityFlags().hebbian);
    EXPECT_TRUE(synapse.getPlasticityFlags().stdp);
    EXPECT_FALSE(synapse.getPlasticityFlags().reward_modulated);
}

TEST(SynapseTest, SynapseFunctionalProperties) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test functional properties
    synapse.setType(nlm::SynapseType::Excitatory);
    EXPECT_TRUE(synapse.isFunctional());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    EXPECT_TRUE(synapse.isFunctional());
    
    synapse.setWeight(0.0);
    EXPECT_FALSE(synapse.isFunctional());
}

TEST(SynapseTest, SynapseConnectionProperties) {
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(1), nlm::NeuronId(2));
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(1), nlm::NeuronId(3));
    
    // Same synapse properties
    EXPECT_EQ(synapse1.getSourceNeuron(), synapse2.getSourceNeuron());
    EXPECT_EQ(synapse1.getDestinationNeuron(), synapse2.getDestinationNeuron());
    EXPECT_NE(synapse1.getDestinationNeuron(), synapse3.getDestinationNeuron());
}

} // namespace

// Backward compatibility - keep the runAll function for existing test_main.cpp
namespace test_synapse {
    void runAll();
}