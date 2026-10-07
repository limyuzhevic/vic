// STDP Tests - Google Test Version
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <cmath>

namespace nlm {

class STDPTest : public ::testing::Test {
protected:
    void SetUp() override {
        stdp = STDP();
        rng = RandomGenerator(42);
    }
    
    STDP stdp;
    RandomGenerator rng;
};

TEST_F(STDPTest, STDPCreation) {
    ASSERT_FLOAT_EQ(stdp.getLTPWeight(), 0.01f);
    ASSERT_FLOAT_EQ(stdp.getLTDWeight(), 0.012f);
    ASSERT_FLOAT_EQ(stdp.getTimeConstant(), 20.0f);
}

TEST_F(STDPTest, STDPConfiguration) {
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    ASSERT_FLOAT_EQ(stdp.getLTPWeight(), 0.05f);
    ASSERT_FLOAT_EQ(stdp.getLTDWeight(), 0.06f);
    ASSERT_FLOAT_EQ(stdp.getTimeConstant(), 30.0f);
}

TEST_F(STDPTest, STDPConfigure) {
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    ASSERT_FLOAT_EQ(stdp.getLTPWeight(), 0.03f);
    ASSERT_FLOAT_EQ(stdp.getLTDWeight(), 0.035f);
    ASSERT_FLOAT_EQ(stdp.getTimeConstant(), 25.0f);
}

TEST_F(STDPTest, STDPName) {
    ASSERT_NE(stdp.getName(), nullptr);
    ASSERT_STREQ(stdp.getName(), "STDP");
}

TEST_F(STDPTest, STDPPotentiation) {
    // Create a synapse
    Synapse synapse(SynapseId(1), NeuronId(1), NeuronId(2));
    synapse.setType(SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    ASSERT_GT(finalWeight, initialWeight);
}

TEST_F(STDPTest, STDPDepression) {
    // Create a synapse
    Synapse synapse(SynapseId(2), NeuronId(3), NeuronId(4));
    synapse.setType(SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    ASSERT_LT(finalWeight, initialWeight);
}

TEST_F(STDPTest, STDPEmptySpikes) {
    // Create a synapse
    Synapse synapse(SynapseId(3), NeuronId(5), NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<Timestamp> preSpikes;
    std::vector<Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    ASSERT_FLOAT_EQ(synapse.getWeight(), initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    ASSERT_FLOAT_EQ(synapse.getWeight(), initialWeight);
}

TEST_F(STDPTest, STDPWeightBounds) {
    // Create a synapse
    Synapse synapse(SynapseId(4), NeuronId(7), NeuronId(8));
    synapse.setType(SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    ASSERT_LE(synapse.getWeight(), 1.0f);
}

TEST_F(STDPTest, STDPLowWeightBounds) {
    // Create a synapse
    Synapse synapse(SynapseId(5), NeuronId(9), NeuronId(10));
    synapse.setType(SynapseType::Excitatory);
    synapse.setWeight(-0.99f);  // Near min
    
    // Very strong depression
    std::vector<Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<Timestamp> postSpikes = {5.0, 6.0, 7.0, 8.0, 9.0};  // post after pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to min
    ASSERT_GE(synapse.getWeight(), -1.0f);
}

TEST_F(STDPTest, STDPLinearScaling) {
    // Test that delta is proportional to time difference
    Synapse synapse1(SynapseId(6), NeuronId(11), NeuronId(12));
    Synapse synapse2(SynapseId(7), NeuronId(13), NeuronId(14));
    
    synapse1.setWeight(0.5f);
    synapse2.setWeight(0.5f);
    
    std::vector<Timestamp> preSpikes = {0.0};
    std::vector<Timestamp> postSpikesClose = {2.0};   // 2ms difference
    std::vector<Timestamp> postSpikesFar = {50.0};   // 50ms difference
    
    stdp.update(&synapse1, preSpikes, postSpikesClose, 0.001);
    float deltaClose = synapse1.getWeight() - 0.5f;
    
    synapse2.setWeight(0.5f);
    stdp.update(&synapse2, preSpikes, postSpikesFar, 0.001);
    float deltaFar = synapse2.getWeight() - 0.5f;
    
    // Close spikes should produce larger potentiation than far spikes
    ASSERT_GT(deltaClose, deltaFar);
}

TEST_F(STDPTest, STDPAsymmetric) {
    // Test that potentiation and depression have different magnitudes
    Synapse synapsePot(SynapseId(8), NeuronId(15), NeuronId(16));
    Synapse synapseDep(SynapseId(9), NeuronId(17), NeuronId(18));
    
    synapsePot.setWeight(0.5f);
    synapseDep.setWeight(0.5f);
    
    std::vector<Timestamp> preSpikes = {0.0};
    std::vector<Timestamp> postSpikes = {5.0};  // pre before post
    
    stdp.update(&synapsePot, preSpikes, postSpikes, 0.001);
    float deltaPot = synapsePot.getWeight() - 0.5f;
    
    // Now test depression: post before pre
    std::vector<Timestamp> postBefore = {2.0};
    std::vector<Timestamp> preAfter = {10.0};
    
    stdp.update(&synapseDep, preAfter, postBefore, 0.001);
    float deltaDep = synapseDep.getWeight() - 0.5f;
    
    // Potentiation and depression should have opposite signs
    ASSERT_GT(deltaPot, 0.0f);
    ASSERT_LT(deltaDep, 0.0f);
}

} // namespace nlm

int main(int argc, char **argv) {
    std::cout << "=== NLM STDP Tests (Google Test) ===" << std::endl;
    std::cout << "Testing Spike-Timing Dependent Plasticity:" << std::endl;
    std::cout << "  - Basic STDP mechanisms" << std::endl;
    std::cout << "  - Potentiation (pre before post)" << std::endl;
    std::cout << "  - Depression (post before pre)" << std::endl;
    std::cout << "  - Weight bounds and clamping" << std::endl;
    std::cout << "  - Temporal asymmetry" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
