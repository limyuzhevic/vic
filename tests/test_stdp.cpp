// STDP Tests - GoogleTest Version
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include "GoogleTestHelper.hpp"
#include "TestMatchers.hpp"

// TEST SUITE
class STDPTestSuite : public ::testing::Test {
protected:
    nlm::STDP stdp;
    nlm::Synapse synapse;
    
    void SetUp() override {
        synapse = nlm::Synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
    }
};

// STDP TESTS
TEST_F(STDPTestSuite, STDPCreation) {
    nlm::STDP stdp;
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.01f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.012f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 20.0f);
}

TEST_F(STDPTestSuite, STDPConfiguration) {
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.05f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.06f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 30.0f);
}

TEST_F(STDPTestSuite, STDPConfigure) {
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.03f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.035f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 25.0f);
}

TEST_F(STDPTestSuite, STDPName) {
    EXPECT_NE(stdp.getName(), nullptr);
    EXPECT_EQ(std::string(stdp.getName()), "STDP");
}

TEST_F(STDPTestSuite, STDPPotentiation) {
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    EXPECT_GT(finalWeight, initialWeight);
}

TEST_F(STDPTestSuite, STDPDepression) {
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    float initialWeight = synapse2.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse2, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse2.getWeight();
    EXPECT_LT(finalWeight, initialWeight);
}

TEST_F(STDPTestSuite, STDPEmptySpikes) {
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse3.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse3, preSpikes, postSpikes, 0.001);
    EXPECT_FLOAT_EQ(synapse3.getWeight(), initialWeight);
    
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse3, preSpikes, postSpikes, 0.001);
    EXPECT_FLOAT_EQ(synapse3.getWeight(), initialWeight);
}

TEST_F(STDPTestSuite, STDPWeightBounds) {
    nlm::Synapse synapse4(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse4.setType(nlm::SynapseType::Excitatory);
    synapse4.setWeight(0.99f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse4, preSpikes, postSpikes, 0.001);
    
    EXPECT_LE(synapse4.getWeight(), 1.0f);
}
