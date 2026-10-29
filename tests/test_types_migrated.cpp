// Migrated test_types.cpp using the new test framework
// This file replaces the original test_types.cpp

#include "core/Types/Types.hpp"
#include "nlm/test/TestFramework.hpp"
using namespace nlm::test;

TEST_CASE("Types", "testNeuronId") {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    TEST_ASSERT_EQ(id1, id3);
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id2, id3);
    TEST_ASSERT_GT(id1, id2);
    TEST_ASSERT_EQ(id1.index(), 1);
}

TEST_CASE("Types", "testSynapseId") {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
}

TEST_CASE("Types", "testRegionId") {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
}

TEST_CASE("Types", "testPopulationId") {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
}

TEST_CASE("Types", "testNeuronType") {
    TEST_ASSERT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    TEST_ASSERT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor);
}

TEST_CASE("Types", "testSynapseType") {
    TEST_ASSERT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
}

TEST_CASE("Types", "testPlasticityFlags") {
    nlm::PlasticityFlags flags;
    TEST_ASSERT_FALSE(flags.hebbian);
    TEST_ASSERT_FALSE(flags.stdp);
    TEST_ASSERT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    TEST_ASSERT_TRUE(flags.hebbian);
}

TEST_CASE("Types", "testSpikeEvent") {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    TEST_ASSERT_EQ(event.source_neuron.index(), 1);
    TEST_ASSERT_EQ(event.timestamp, 0.5);
    TEST_ASSERT_EQ(event.step, 500);
}
