// Types Tests - Google Test Version
#include "core/Types/Types.hpp"
#include <gtest/gtest.h>
#include <iostream>

namespace nlm {

class TypesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test data
        id1 = NeuronId(1);
        id2 = NeuronId(2);
        id3 = NeuronId(1);
        syn1 = SynapseId(100);
        syn2 = SynapseId(200);
        reg1 = RegionId(1);
        reg2 = RegionId(2);
        pop1 = PopulationId(1);
        pop2 = PopulationId(2);
    }
    
    NeuronId id1, id2, id3;
    SynapseId syn1, syn2;
    RegionId reg1, reg2;
    PopulationId pop1, pop2;
};

TEST_F(TypesTest, NeuronIdEquality) {
    ASSERT_EQ(id1, id3);
    ASSERT_NE(id1, id2);
    ASSERT_LT(id1, id2);
    ASSERT_GT(id2, id1);
    ASSERT_EQ(id1.index(), 1);
}

TEST_F(TypesTest, NeuronIdHashing) {
    ASSERT_EQ(std::hash<NeuronId>()(id1), std::hash<uint64_t>()(id1.value));
    ASSERT_NE(std::hash<NeuronId>()(id1), std::hash<NeuronId>()(id2));
}

TEST_F(TypesTest, SynapseIdComparison) {
    ASSERT_NE(syn1, syn2);
    ASSERT_LT(syn1, syn2);
    ASSERT_GT(syn2, syn1);
}

TEST_F(TypesTest, RegionIdComparison) {
    ASSERT_NE(reg1, reg2);
    ASSERT_LT(reg1, reg2);
    ASSERT_GT(reg2, reg1);
}

TEST_F(TypesTest, PopulationIdComparison) {
    ASSERT_NE(pop1, pop2);
    ASSERT_LT(pop1, pop2);
    ASSERT_GT(pop2, pop1);
}

TEST_F(TypesTest, NeuronIdAssignment) {
    NeuronId id4 = id1;
    ASSERT_EQ(id4, id1);
    ASSERT_EQ(id4.index(), 1);
}

TEST_F(TypesTest, SynapseIdAssignment) {
    SynapseId syn3 = syn1;
    ASSERT_EQ(syn3, syn1);
    ASSERT_EQ(syn3.value, 100);
}

TEST_F(TypesTest, NeuronTypeValues) {
    // Test that enum values are distinct and accessible
    ASSERT_NE(NeuronType::Excitatory, NeuronType::Inhibitory);
    ASSERT_NE(NeuronType::Sensory, NeuronType::Motor);
    ASSERT_NE(NeuronType::Internal, NeuronType::Modulatory);
    
    // Test that we can cast to integers
    ASSERT_EQ(static_cast<int>(NeuronType::Excitatory), 0);
    ASSERT_EQ(static_cast<int>(NeuronType::Inhibitory), 1);
    ASSERT_EQ(static_cast<int>(NeuronType::Internal), 5);
}

TEST_F(TypesTest, SynapseTypeValues) {
    ASSERT_NE(SynapseType::Excitatory, SynapseType::Inhibitory);
    ASSERT_NE(SynapseType::Electrical, SynapseType::Modulatory);
    ASSERT_NE(SynapseType::GapJunction, SynapseType::Electrical);
    
    // Test integer values
    ASSERT_EQ(static_cast<int>(SynapseType::Excitatory), 0);
    ASSERT_EQ(static_cast<int>(SynapseType::Inhibitory), 1);
    ASSERT_EQ(static_cast<int>(SynapseType::Electrical), 2);
}

TEST_F(TypesTest, PlasticityFlags) {
    PlasticityFlags flags;
    ASSERT_FALSE(flags.hebbian);
    ASSERT_FALSE(flags.stdp);
    ASSERT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    ASSERT_TRUE(flags.hebbian);
    ASSERT_FALSE(flags.stdp);
    
    flags.stdp = true;
    flags.reward_modulated = true;
    ASSERT_TRUE(flags.stdp);
    ASSERT_TRUE(flags.reward_modulated);
}

TEST_F(TypesTest, SpikeEvent) {
    SpikeEvent event;
    event.source_neuron = NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    ASSERT_EQ(event.source_neuron.index(), 1);
    ASSERT_FLOAT_EQ(event.timestamp, 0.5);
    ASSERT_EQ(event.step, 500);
    
    // Test field accessibility
    ASSERT_TRUE(event.source_neuron.value == 1);
}

TEST_F(TypesTest, SpikeEventAssignment) {
    SpikeEvent event1;
    event1.source_neuron = NeuronId(42);
    event1.timestamp = 2.5;
    event1.step = 2500;
    
    SpikeEvent event2 = event1;
    ASSERT_EQ(event2.source_neuron, event1.source_neuron);
    ASSERT_FLOAT_EQ(event2.timestamp, event1.timestamp);
    ASSERT_EQ(event2.step, event1.step);
}

TEST_F(TypesTest, TypeLimits) {
    // Test ID limits (should be unsigned, max value is UINT64_MAX)
    NeuronId maxId(UINT64_MAX);
    SynapseId maxSynId(UINT64_MAX);
    RegionId maxRegId(UINT64_MAX);
    PopulationId maxPopId(UINT64_MAX);
    
    // IDs should be valid
    ASSERT_TRUE(maxId.value == UINT64_MAX);
    ASSERT_TRUE(maxSynId.value == UINT64_MAX);
    ASSERT_TRUE(maxRegId.value == UINT64_MAX);
    ASSERT_TRUE(maxPopId.value == UINT64_MAX);
}

TEST_F(TypesTest, DefaultConstruction) {
    NeuronId defaultId;
    SynapseId defaultSynId;
    RegionId defaultRegId;
    PopulationId defaultPopId;
    
    ASSERT_EQ(defaultId.value, 0);
    ASSERT_EQ(defaultSynId.value, 0);
    ASSERT_EQ(defaultRegId.value, 0);
    ASSERT_EQ(defaultPopId.value, 0);
}

TEST_F(TypesTest, InvalidIDs) {
    // Test that INVALID_*_ID constants exist and have expected values
    ASSERT_EQ(INVALID_NEURON_ID.value, static_cast<uint64_t>(-1));
    ASSERT_EQ(INVALID_SYNAPSE_ID.value, static_cast<uint64_t>(-1));
    ASSERT_EQ(INVALID_REGION_ID.value, static_cast<uint64_t>(-1));
    ASSERT_EQ(INVALID_POPULATION_ID.value, static_cast<uint64_t>(-1));
}

TEST_F(TypesTest, ConstCorrectness) {
    const NeuronId constId(42);
    
    // Should be able to read value
    uint64_t val = constId.value;
    ASSERT_EQ(val, 42);
    
    // Should be able to call methods
    size_t idx = constId.index();
    ASSERT_EQ(idx, 42);
}

} // namespace nlm

int main(int argc, char **argv) {
    std::cout << "=== NLM Types Tests (Google Test) ===" << std::endl;
    std::cout << "Testing Type System and Data Structures:" << std::endl;
    std::cout << "  - Neuron ID management and comparison" << std::endl;
    std::cout << "  - Synapse ID handling" << std::endl;
    std::cout << "  - Region and Population ID systems" << std::endl;
    std::cout << "  - Enumeration values and distinctness" << std::endl;
    std::cout << "  - Plasticity flags configuration" << std::endl;
    std::cout << "  - Spike event data structure" << std::endl;
    std::cout << "  - Default values and limits" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
