// Types Tests
// Test type definitions and utilities

#include <gtest/gtest.h>
#include "core/Types/Types.hpp"

namespace nlm {

TEST(TypesTest, BasicDefinitions) {
    NeuronId neuronId = NeuronId(1);
    EXPECT_EQ(neuronId.getId(), 1);
    
    SynapseId synapseId = SynapseId(2);
    EXPECT_EQ(synapseId.getId(), 2);
    
    RegionId regionId = RegionId(3);
    EXPECT_EQ(regionId.getId(), 3);
}

TEST(TypesTest, TypeConversions) {
    NeuronId id1 = NeuronId(1);
    NeuronId id2 = NeuronId(id1);
    EXPECT_EQ(id1.getId(), id2.getId());
}

TEST(TypesTest, InvalidId) {
    NeuronId invalidId;
    EXPECT_EQ(invalidId.getId(), 0);
}

} // namespace nlm

