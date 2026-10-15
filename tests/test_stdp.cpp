// STDP Tests - GoogleTest Version
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <cmath>

namespace {

TEST(STDPTTest, STDPCreation) {
    nlm::STDP stdp;
    
    EXPECT_DOUBLE_EQ(stdp.getLTPWeight(), 0.01);
    EXPECT_DOUBLE_EQ(stdp.getLTDWeight(), 0.012);
    EXPECT_DOUBLE_EQ(stdp.getTimeConstant(), 20.0);
}

TEST(STDPTTest, STDPConfiguration) {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05);
    stdp.setLTDWeight(0.06);
    stdp.setTimeConstant(30.0);
    
    EXPECT_DOUBLE_EQ(stdp.getLTPWeight(), 0.05);
    EXPECT_DOUBLE_EQ(stdp.getLTDWeight(), 0.06);
    EXPECT_DOUBLE_EQ(stdp.getTimeConstant(), 30.0);
}

TEST(STDPTTest, STDPConfigure) {
    nlm::STDP stdp;
    
    stdp.configure(0.03, 0.035, 25.0);
    
    EXPECT_DOUBLE_EQ(stdp.getLTPWeight(), 0.03);
    EXPECT_DOUBLE_EQ(stdp.getLTDWeight(), 0.035);
    EXPECT_DOUBLE_EQ(stdp.getTimeConstant(), 25.0);
}

TEST(STDPTTest, STDPName) {
    nlm::STDP stdp;
    
    const char* name = stdp.getName();
    EXPECT_NE(name, nullptr);
    EXPECT_STREQ(name, "STDP");
}

TEST(STDPTTest, STDPPotentiation) {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5);
    
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    EXPECT_GT(finalWeight, initialWeight);
}

TEST(STDPTTest, STDPDepression) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5);
    
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    EXPECT_LT(finalWeight, initialWeight);
}

TEST(STDPTTest, STDPEmptySpikes) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), initialWeight);
    
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    EXPECT_DOUBLE_EQ(synapse.getWeight(), initialWeight);
}

TEST(STDPTTest, STDPWeightBounds) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    EXPECT_LE(synapse.getWeight(), 1.0);
}

TEST(STDPTTest, STDPEffectiveness) {
    nlm::STDP stdp;
    
    // Test different time windows
    stdp.setTimeConstant(10.0);
    EXPECT_DOUBLE_EQ(stdp.getTimeConstant(), 10.0);
    
    stdp.setTimeConstant(50.0);
    EXPECT_DOUBLE_EQ(stdp.getTimeConstant(), 50.0);
    
    // Test weight scaling
    stdp.setLTPWeight(0.1);
    EXPECT_DOUBLE_EQ(stdp.getLTPWeight(), 0.1);
    
    stdp.setLTDWeight(0.2);
    EXPECT_DOUBLE_EQ(stdp.getLTDWeight(), 0.2);
}

TEST(STDPTTest, STDPAdaptation) {
    nlm::STDP stdp1;
    nlm::STDP stdp2;
    
    // Different configurations should produce different effects
    stdp1.configure(0.01, 0.02, 20.0);
    stdp2.configure(0.05, 0.06, 30.0);
    
    EXPECT_DOUBLE_EQ(stdp1.getLTPWeight(), 0.01);
    EXPECT_DOUBLE_EQ(stdp2.getLTPWeight(), 0.05);
    EXPECT_LT(stdp1.getLTPWeight(), stdp2.getLTPWeight());
}

TEST(STDPTTest, STDPIntegrationWithSynapse) {
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5);
    
    // Apply STDP with various spike patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    float weightBefore = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float weightAfter = synapse.getWeight();
    
    // Either potentiation or depression should occur
    EXPECT_TRUE(weightAfter != weightBefore);
}

} // namespace

// Backward compatibility - keep the runAll function for existing test_main.cpp
namespace test_stdp {
    void runAll();
}