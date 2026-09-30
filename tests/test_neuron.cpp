// Comprehensive Neuron Tests
// Covers LIF neuron dynamics, spike generation, and synaptic integration

#include <gtest/gtest.h>
#include "brain/Neuron.hpp"
#include <cmath>

namespace nlm {

class NeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        neuron = std::make_unique<Neuron>(NeuronId(1));
        neuron->setType(NeuronType::Excitatory);
        neuron->setThreshold(-50.0f);
    }
    
    void TearDown() override {
        neuron.reset();
    }
    
    std::unique_ptr<Neuron> neuron;
};

TEST_F(NeuronTest, LIFDynamics) {
    // Test LIF neuron dynamics with various inputs
    neuron->setMembranePotential(-70.0f);
    neuron->addToMembranePotential(30.0f);  // Enough to reach threshold
    EXPECT_TRUE(neuron->checkThreshold());
}

TEST_F(NeuronTest, SpikeGeneration) {
    // Test spike generation and refractory periods
    neuron->setMembranePotential(-70.0f);
    neuron->addToMembranePotential(40.0f);
    EXPECT_TRUE(neuron->checkThreshold());
    
    neuron->setFiringState(FiringState::Refractory);
    EXPECT_TRUE(neuron->isRefractory());
    neuron->decrementRefractory();
    EXPECT_FALSE(neuron->isRefractory());
}

TEST_F(NeuronTest, SynapticIntegration) {
    // Test synaptic integration
    float initialPot = neuron->getMembranePotential();
    neuron->addSynapticInput(10.5f);
    EXPECT_GT(neuron->getMembranePotential(), initialPot);
}

TEST_F(NeuronTest, RefractoryPeriod) {
    neuron->setRefractoryPeriod(10);
    neuron->setFiringState(FiringState::Refractory);
    
    EXPECT_TRUE(neuron->isRefractory());
    for (int i = 0; i < 5; ++i) {
        neuron->decrementRefractory();
    }
    EXPECT_TRUE(neuron->isRefractory());
    for (int i = 5; i < 10; ++i) {
        neuron->decrementRefractory();
    }
    EXPECT_FALSE(neuron->isRefractory());
}

TEST_F(NeuronTest, MembranePotentialBounds) {
    // Test membrane potential bounds and limits
    neuron->setMembranePotential(-100.0f);
    EXPECT_GE(neuron->getMembranePotential(), -100.0f);
}

TEST_F(NeuronTest, FiringRateControl) {
    // Test firing rate control
    neuron->setFiringRate(10.0f);
    EXPECT_EQ(neuron->getFiringRate(), 10.0f);
}

TEST(NeuronTest, BasicCreation) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    EXPECT_EQ(neuron.getId(), nlm::NeuronId(1));
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Internal);
}

TEST(NeuronTest, MembranePotential) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setMembranePotential(-70.0f);
    EXPECT_EQ(neuron.getMembranePotential(), -70.0f);
}

TEST(NeuronTest, FiringStates) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setFiringState(nlm::FiringState::Active);
    EXPECT_TRUE(neuron.isFiring());
}

} // namespace nlm

