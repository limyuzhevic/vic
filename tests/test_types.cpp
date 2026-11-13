// Types Tests
#include "core/Types/Types.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(TypesTest, NeuronId) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    EXPECT_EQ(id1, id3);
    EXPECT_NE(id1, id2);
    EXPECT_LT(id1, id2);
    EXPECT_GT(id2, id1);
    EXPECT_EQ(id1.index(), 1);
    
    std::cout << "    testNeuronId passed" << std::endl;
}

TEST(TypesTest, SynapseId) {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    EXPECT_NE(id1, id2);
    EXPECT_LT(id1, id2);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

TEST(TypesTest, RegionId) {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    EXPECT_NE(id1, id2);
    
    std::cout << "    testRegionId passed" << std::endl;
}

TEST(TypesTest, PopulationId) {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    EXPECT_NE(id1, id2);
    
    std::cout << "    testPopulationId passed" << std::endl;
}

TEST(TypesTest, NeuronType) {
    EXPECT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    EXPECT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

TEST(TypesTest, SynapseType) {
    EXPECT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

TEST(TypesTest, PlasticityFlags) {
    nlm::PlasticityFlags flags;
    EXPECT_FALSE(flags.hebbian);
    EXPECT_FALSE(flags.stdp);
    EXPECT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    EXPECT_TRUE(flags.hebbian);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

TEST(TypesTest, SpikeEvent) {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    EXPECT_EQ(event.source_neuron.index(), 1);
    EXPECT_DOUBLE_EQ(event.timestamp, 0.5);
    EXPECT_EQ(event.step, 500);
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "=== NLM Types Tests ===" << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    
    std::cout << "=== Types Tests Complete ===" << std::endl;
    return result;
}