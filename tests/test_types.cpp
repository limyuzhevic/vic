// Types Tests - GoogleTest Version
#include "core/Types/Types.hpp"
#include <gtest/gtest.h>
#include "GoogleTestHelper.hpp"
#include "TestMatchers.hpp"

// TEST SUITE
class TypesTestSuite : public ::testing::Test {
protected:
    nlm::NeuronId neuronId1;
    nlm::NeuronId neuronId2;
    nlm::SynapseId synapseId;
    nlm::RegionId regionId;
    nlm::PopulationId populationId;
    
    void SetUp() override {
        neuronId1 = nlm::NeuronId(1);
        neuronId2 = nlm::NeuronId(2);
        synapseId = nlm::SynapseId(100);
        regionId = nlm::RegionId(1);
        populationId = nlm::PopulationId(1);
    }
};

// TYPES TESTS
TEST_F(TypesTestSuite, NeuronId) {
    EXPECT_EQ(neuronId1, nlm::NeuronId(1));
    EXPECT_NE(neuronId1, neuronId2);
    EXPECT_LT(neuronId1, neuronId2);
    EXPECT_GT(neuronId2, neuronId1);
    EXPECT_EQ(neuronId1.index(), 1);
}

TEST_F(TypesTestSuite, SynapseId) {
    EXPECT_NE(synapseId, nlm::SynapseId(200));
    EXPECT_LT(synapseId, nlm::SynapseId(200));
    EXPECT_GT(nlm::SynapseId(200), synapseId);
}

TEST_F(TypesTestSuite, RegionId) {
    EXPECT_NE(regionId, nlm::RegionId(2));
}

TEST_F(TypesTestSuite, PopulationId) {
    EXPECT_NE(populationId, nlm::PopulationId(2));
}

TEST_F(TypesTestSuite, NeuronType) {
    EXPECT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    EXPECT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor);
}

TEST_F(TypesTestSuite, SynapseType) {
    EXPECT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
}

TEST_F(TypesTestSuite, PlasticityFlags) {
    nlm::PlasticityFlags flags;
    EXPECT_FALSE(flags.hebbian);
    EXPECT_FALSE(flags.stdp);
    EXPECT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    EXPECT_TRUE(flags.hebbian);
}

TEST_F(TypesTestSuite, SpikeEvent) {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    EXPECT_EQ(event.source_neuron.index(), 1);
    EXPECT_DOUBLE_EQ(event.timestamp, 0.5);
    EXPECT_EQ(event.step, 500);
}
