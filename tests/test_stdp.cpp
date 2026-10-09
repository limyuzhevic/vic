// STDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include "nlm/test_framework.h"
#include <iostream>
#include <cmath>

namespace test_stdp {

TEST_CASE(testSTDPCreation) {
    nlm::STDP stdp;
    
    ASSERT_FLOAT_EQ(stdp.getLTPWeight(), 0.01f, 0.0001f);
    ASSERT_FLOAT_EQ(stdp.getLTDWeight(), 0.012f, 0.0001f);
    ASSERT_FLOAT_EQ(stdp.getTimeConstant(), 20.0f, 0.001f);
}

TEST_CASE(testSTDPConfiguration) {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    ASSERT_FLOAT_EQ(stdp.getLTPWeight(), 0.05f, 0.0001f);
    ASSERT_FLOAT_EQ(stdp.getLTDWeight(), 0.06f, 0.0001f);
    ASSERT_FLOAT_EQ(stdp.getTimeConstant(), 30.0f, 0.001f);
}

TEST_CASE(testSTDPConfigure) {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    ASSERT_FLOAT_EQ(stdp.getLTPWeight(), 0.03f, 0.0001f);
    ASSERT_FLOAT_EQ(stdp.getLTDWeight(), 0.035f, 0.0001f);
    ASSERT_FLOAT_EQ(stdp.getTimeConstant(), 25.0f, 0.001f);
}

TEST_CASE(testSTDPName) {
    nlm::STDP stdp;
    
    ASSERT_TRUE(stdp.getName() != nullptr);
    ASSERT_EQ(std::string(stdp.getName()), std::string("STDP"));
}

TEST_CASE(testSTDPPotentiation) {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    ASSERT_TRUE(finalWeight > initialWeight);
}

TEST_CASE(testSTDPDepression) {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    ASSERT_TRUE(finalWeight < initialWeight);
}

TEST_CASE(testSTDPEmptySpikes) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    ASSERT_FLOAT_EQ(synapse.getWeight(), initialWeight, 0.0001f);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    ASSERT_FLOAT_EQ(synapse.getWeight(), initialWeight, 0.0001f);
}

TEST_CASE(testSTDPWeightBounds) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    ASSERT_LE(synapse.getWeight(), 1.0f);
}

std::vector<TestResult> runAllTests() {
    return test_stdp::runAllTests();
}

} // namespace test_stdp

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_stdp::runAllTests();
    printTestSummary(results);
}
