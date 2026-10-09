// Types Tests
#include "core/Types/Types.hpp"
#include "nlm/test_framework.h"
#include <iostream>

namespace test_types {

TEST_CASE(testNeuronId) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    ASSERT_EQ(id1, id3);
    ASSERT_NE(id1, id2);
    ASSERT_GT(id2, id1);
    ASSERT_LT(id1, id2);
    ASSERT_EQ(id1.index(), 1);
}

TEST_CASE(testSynapseId) {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    ASSERT_NE(id1, id2);
    ASSERT_LT(id1, id2);
}

TEST_CASE(testRegionId) {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    ASSERT_NE(id1, id2);
}

TEST_CASE(testPopulationId) {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    ASSERT_NE(id1, id2);
}

TEST_CASE(testNeuronType) {
    ASSERT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    ASSERT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor);
}

TEST_CASE(testSynapseType) {
    ASSERT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
}

TEST_CASE(testPlasticityFlags) {
    nlm::PlasticityFlags flags;
    ASSERT_FALSE(flags.hebbian);
    ASSERT_FALSE(flags.stdp);
    ASSERT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    ASSERT_TRUE(flags.hebbian);
}

TEST_CASE(testSpikeEvent) {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    ASSERT_EQ(event.source_neuron.index(), 1);
    ASSERT_FLOAT_EQ(event.timestamp, 0.5, 0.001f);
    ASSERT_EQ(event.step, 500);
}

std::vector<TestResult> runAllTests() {
    return test_types::runAllTests();
}

} // namespace test_types

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_types::runAllTests();
    printTestSummary(results);
}
