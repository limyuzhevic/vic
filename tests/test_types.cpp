// Types Tests - GoogleTest Version
#include "core/Types/Types.hpp"
#include <gtest/gtest.h>
#include <string>

namespace {

TEST(TypesTest, NeuronIdOperations) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    EXPECT_EQ(id1, id3);
    EXPECT_NE(id1, id2);
    EXPECT_LT(id1, id2);
    EXPECT_GT(id2, id1);
    EXPECT_EQ(id1.index(), 1);
}

TEST(TypesTest, SynapseIdOperations) {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    EXPECT_NE(id1, id2);
    EXPECT_LT(id1, id2);
}

TEST(TypesTest, RegionIdOperations) {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    EXPECT_NE(id1, id2);
}

TEST(TypesTest, PopulationIdOperations) {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    EXPECT_NE(id1, id2);
}

TEST(TypesTest, NeuronTypeEnumeration) {
    EXPECT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    EXPECT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor);
}

TEST(TypesTest, SynapseTypeEnumeration) {
    EXPECT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
}

TEST(TypesTest, PlasticityFlags) {
    nlm::PlasticityFlags flags;
    EXPECT_FALSE(flags.hebbian);
    EXPECT_FALSE(flags.stdp);
    EXPECT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    EXPECT_TRUE(flags.hebbian);
}

TEST(TypesTest, SpikeEvent) {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    EXPECT_EQ(event.source_neuron.index(), 1);
    EXPECT_DOUBLE_EQ(event.timestamp, 0.5);
    EXPECT_EQ(event.step, 500);
}

TEST(TypesTest, AllTypesTest) {
    // Comprehensive test to ensure all type functionality works
    nlm::NeuronId neuronId(1);
    nlm::SynapseId synapseId(100);
    nlm::RegionId regionId(1);
    nlm::PopulationId populationId(1);
    
    EXPECT_TRUE(neuronId.isValid());
    EXPECT_TRUE(synapseId.isValid());
    EXPECT_TRUE(regionId.isValid());
    EXPECT_TRUE(populationId.isValid());
    
    // Test that each type can be default constructed
    nlm::Neuron neuron(neuronId);
    nlm::Synapse synapse(synapseId, neuronId, neuronId);
    
    EXPECT_EQ(neuron.getId(), neuronId);
    EXPECT_EQ(synapse.getId(), synapseId);
}

} // namespace

// Backward compatibility - keep the runAll function for existing test_main.cpp
namespace test_types {
    void runAll();
}