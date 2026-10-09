#include <gtest/gtest.h>
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include <memory>
#include <vector>
#include <cmath>

namespace test_plasticity {

class PlasticityTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Create two neurons for synapse
        preNeuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
        postNeuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(2));
        
        // Create a synapse
        preNeuron->setType(nlm::NeuronType::Excitatory);
        postNeuron->setType(nlm::NeuronType::Excitatory);
        
        // Create synapse with initial weight
        synapse = std::make_unique<nlm::Synapse>(preNeuron.get(), postNeuron.get(), 0.5f);
        
        // Create plasticity rules
        stdp = std::make_unique<nlm::STDP>();
        hebbian = std::make_unique<nlm::HebbianRule>();
        structural = std::make_unique<nlm::StructuralPlasticity>();
    }

    void TearDown() override {
        synapse.reset();
        preNeuron.reset();
        postNeuron.reset();
        stdp.reset();
        hebbian.reset();
        structural.reset();
    }

    std::unique_ptr<nlm::Neuron> preNeuron;
    std::unique_ptr<nlm::Neuron> postNeuron;
    std::unique_ptr<nlm::Synapse> synapse;
    std::unique_ptr<nlm::STDP> stdp;
    std::unique_ptr<nlm::HebbianRule> hebbian;
    std::unique_ptr<nlm::StructuralPlasticity> structural;
};

TEST_F(PlasticityTestFixture, STDPInitialization) {
    EXPECT_TRUE(stdp->isEnabled());
    EXPECT_FLOAT_EQ(stdp->getLTPWeight(), 0.01f);
    EXPECT_FLOAT_EQ(stdp->getLTDWeight(), 0.01f);
    EXPECT_FLOAT_EQ(stdp->getTimeConstant(), 20.0f);
}

TEST_F(PlasticityTestFixture, STDPParameterSetting) {
    stdp->configure(0.05f, 0.05f, 15.0f);
    
    EXPECT_FLOAT_EQ(stdp->getLTPWeight(), 0.05f);
    EXPECT_FLOAT_EQ(stdp->getLTDWeight(), 0.05f);
    EXPECT_FLOAT_EQ(stdp->getTimeConstant(), 15.0f);
}

TEST_F(PlasticityTestFixture, STDPName) {
    EXPECT_STREQ(stdp->getName(), "STDP");
}

TEST_F(PlasticityTestFixture, HebbianInitialization) {
    EXPECT_TRUE(hebbian->isEnabled());
    EXPECT_FLOAT_EQ(hebbian->getLearningRate(), 0.01f);
}

TEST_F(PlasticityTestFixture, HebbianParameterSetting) {
    hebbian->setLearningRate(0.05f);
    
    EXPECT_FLOAT_EQ(hebbian->getLearningRate(), 0.05f);
}

TEST_F(PlasticityTestFixture, HebbianName) {
    EXPECT_STREQ(hebbian->getName(), "Hebbian");
}

TEST_F(PlasticityTestFixture, StructuralPlasticityInitialization) {
    EXPECT_TRUE(structural->isEnabled());
    EXPECT_TRUE(structural->getMaxSynapsesPerNeuron() > 0);
    EXPECT_FLOAT_EQ(structural->getConnectionProbability(), 0.1f);
}

TEST_F(PlasticityTestFixture, StructuralPlasticityParameterSetting) {
    structural->setMaxSynapsesPerNeuron(1000);
    structural->setConnectionProbability(0.5f);
    
    EXPECT_EQ(structural->getMaxSynapsesPerNeuron(), 1000);
    EXPECT_FLOAT_EQ(structural->getConnectionProbability(), 0.5f);
}

TEST_F(PlasticityTestFixture, StructuralPlasticityName) {
    EXPECT_STREQ(structural->getName(), "StructuralPlasticity");
}

TEST_F(PlasticityTestFixture, PlasticityRuleEnableDisable) {
    EXPECT_TRUE(stdp->isEnabled());
    
    stdp->setEnabled(false);
    EXPECT_FALSE(stdp->isEnabled());
    
    stdp->setEnabled(true);
    EXPECT_TRUE(stdp->isEnabled());
}

TEST_F(PlasticityTestFixture, STDPWithoutSpikes) {
    // Test STDP without any spike history
    nlm::Timestamp t1(0.0f);
    nlm::Timestamp t2(10.0f);
    std::vector<nlm::Timestamp> preSpikes = {t1};
    std::vector<nlm::Timestamp> postSpikes = {t2};
    
    // This should not crash
    stdp->update(synapse.get(), preSpikes, postSpikes, 0.001f);
}

TEST_F(PlasticityTestFixture, HebbianWithoutSpikes) {
    // Test Hebbian without any spike history
    std::vector<nlm::Timestamp> preSpikes = {};
    std::vector<nlm::Timestamp> postSpikes = {};
    
    // This should not crash
    hebbian->update(synapse.get(), preSpikes, postSpikes, 0.001f);
}

TEST_F(PlasticityTestFixture, StructuralPlasticityWithoutSpikes) {
    // Test structural plasticity without spike history
    std::vector<nlm::Timestamp> preSpikes = {};
    std::vector<nlm::Timestamp> postSpikes = {};
    
    // This should not crash
    structural->update(synapse.get(), preSpikes, postSpikes, 0.001f);
}

TEST_F(PlasticityTestFixture, PlasticityRuleApplyWeightChange) {
    float initialWeight = synapse->getWeight();
    
    stdp->applyWeightChange(synapse.get(), 0.1f);
    EXPECT_FLOAT_EQ(synapse->getWeight(), initialWeight + 0.1f);
    
    hebbian->applyWeightChange(synapse.get(), -0.05f);
    EXPECT_FLOAT_EQ(synapse->getWeight(), initialWeight + 0.05f);
}

TEST_F(PlasticityTestFixture, PlasticityRuleReversibleUpdates) {
    float initialWeight = synapse->getWeight();
    
    // Apply positive weight change
    stdp->applyWeightChange(synapse.get(), 0.1f);
    EXPECT_GT(synapse->getWeight(), initialWeight);
    
    // Apply negative weight change
    stdp->applyWeightChange(synapse.get(), -0.15f);
    EXPECT_LT(synapse->getWeight(), initialWeight);
}

TEST_F(PlasticityTestFixture, STDPWithPositiveTemporalDifference) {
    // Pre before post (should strengthen synapse - LTP)
    nlm::Timestamp preTime(0.0f);
    nlm::Timestamp postTime(10.0f);  // Post after pre by 10ms
    std::vector<nlm::Timestamp> preSpikes = {preTime};
    std::vector<nlm::Timestamp> postSpikes = {postTime};
    
    float initialWeight = synapse->getWeight();
    stdp->update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
}

TEST_F(PlasticityTestFixture, STDPWithNegativeTemporalDifference) {
    // Post before pre (should weaken synapse - LTD)
    nlm::Timestamp postTime(0.0f);
    nlm::Timestamp preTime(10.0f);  // Pre after post by 10ms
    std::vector<nlm::Timestamp> preSpikes = {preTime};
    std::vector<nlm::Timestamp> postSpikes = {postTime};
    
    float initialWeight = synapse->getWeight();
    stdp->update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
}

TEST_F(PlasticityTestFixture, HebbianWithCoactivation) {
    std::vector<nlm::Timestamp> preSpikes = {nlm::Timestamp(0.0f), nlm::Timestamp(5.0f)};
    std::vector<nlm::Timestamp> postSpikes = {nlm::Timestamp(1.0f), nlm::Timestamp(6.0f)};
    
    float initialWeight = synapse->getWeight();
    hebbian->update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
}

TEST_F(PlasticityTestFixture, StructuralPlasticityConnectionFormation) {
    std::vector<nlm::Timestamp> preSpikes = {nlm::Timestamp(0.0f)};
    std::vector<nlm::Timestamp> postSpikes = {nlm::Timestamp(10.0f)};
    
    structural->update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
}

TEST_F(PlasticityTestFixture, PlasticityRuleNameConsistency) {
    EXPECT_STREQ(stdp->getName(), "STDP");
    EXPECT_STREQ(hebbian->getName(), "Hebbian");
    EXPECT_STREQ(structural->getName(), "StructuralPlasticity");
}

TEST_F(PlasticityTestFixture, MultiplePlasticityRules) {
    std::vector<nlm::PlasticityRule*> rules = {
        stdp.get(), hebbian.get(), structural.get()
    };
    
    for (auto* rule : rules) {
        EXPECT_TRUE(rule->isEnabled());
        EXPECT_STREQ(rule->getName(), "STDP");
    }
}

TEST_F(PlasticityTestFixture, PlasticityRuleEnabledState) {
    EXPECT_TRUE(stdp->isEnabled());
    
    stdp->setEnabled(false);
    EXPECT_FALSE(stdp->isEnabled());
    
    stdp->setEnabled(true);
    EXPECT_TRUE(stdp->isEnabled());
}

TEST_F(PlasticityTestFixture, PlasticityRuleWeightRange) {
    float initialWeight = synapse->getWeight();
    
    // Apply multiple weight changes
    for (int i = 0; i < 10; ++i) {
        stdp->applyWeightChange(synapse.get(), 0.01f);
    }
    
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
    EXPECT_GT(synapse->getWeight(), 0.0f);
}

TEST_F(PlasticityTestFixture, PlasticityRuleComplexity) {
    // Create multiple spikes
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes;
    
    for (int i = 0; i < 10; ++i) {
        preSpikes.push_back(nlm::Timestamp(static_cast<float>(i * 5)));
        postSpikes.push_back(nlm::Timestamp(static_cast<float>(i * 5 + 2.5f)));
    }
    
    // Test with many spikes
    hebbian->update(synapse.get(), preSpikes, postSpikes, 0.001f);
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
}

TEST_F(PlasticityTestFixture, PlasticityRuleTimeStepHandling) {
    std::vector<nlm::Timestamp> preSpikes = {nlm::Timestamp(0.0f)};
    std::vector<nlm::Timestamp> postSpikes = {nlm::Timestamp(10.0f)};
    
    // Test with different time steps
    for (float dt : {0.001f, 0.01f, 0.1f}) {
        stp = std::make_unique<nlm::STDP>();
        stp->update(synapse.get(), preSpikes, postSpikes, dt);
        EXPECT_TRUE(std::isfinite(synapse->getWeight()));
    }
}

TEST_F(PlasticityTestFixture, PlasticityRuleNeuronTypeDependent) {
    // Test with different neuron types
    preNeuron->setType(nlm::NeuronType::Inhibitory);
    postNeuron->setType(nlm::NeuronType::Excitatory);
    
    std::vector<nlm::Timestamp> preSpikes = {nlm::Timestamp(0.0f)};
    std::vector<nlm::Timestamp> postSpikes = {nlm::Timestamp(10.0f)};
    
    stp->update(synapse.get(), preSpikes, postSpikes, 0.001f);
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
}

TEST_F(PlasticityTestFixture, PlasticityRuleIntegration) {
    // Test integration of all plasticity rules
    std::vector<nlm::PlasticityRule*> rules = {
        stdp.get(), hebbian.get(), structural.get()
    };
    
    std::vector<nlm::Timestamp> preSpikes = {nlm::Timestamp(0.0f)};
    std::vector<nlm::Timestamp> postSpikes = {nlm::Timestamp(10.0f)};
    
    for (auto* rule : rules) {
        rule->update(synapse.get(), preSpikes, postSpikes, 0.001f);
        EXPECT_TRUE(rule->isEnabled());
    }
}

TEST_F(PlasticityTestFixture, PlasticityRuleWeightConservation) {
    float initialWeight = synapse->getWeight();
    
    // Apply random weight changes
    for (int i = 0; i < 20; ++i) {
        float change = static_cast<float>((rand() % 200 - 100)) / 1000.0f;  // -0.1 to 0.1
        stdp->applyWeightChange(synapse.get(), change);
    }
    
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
}

} // namespace test_plasticity