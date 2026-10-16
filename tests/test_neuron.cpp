// Neuron Tests - GoogleTest Version
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include "GoogleTestHelper.hpp"
#include "TestMatchers.hpp"

// TEST SUITE
class NeuronTestSuite : public ::testing::Test {
protected:
    nlm::Neuron neuron;
    nlm::RandomGenerator rng;
    
    void SetUp() override {
        neuron = nlm::Neuron(nlm::NeuronId(1));
        rng.seed(42);
    }
};

// NEURON TESTS
TEST_F(NeuronTestSuite, NeuronCreation) {
    EXPECT_EQ(neuron.getId(), nlm::NeuronId(1));
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Internal);
}

TEST_F(NeuronTestSuite, NeuronType) {
    neuron.setType(nlm::NeuronType::Excitatory);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Inhibitory);
}

TEST_F(NeuronTestSuite, MembranePotential) {
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -70.0f);
    
    neuron.setMembranePotential(-65.0f);
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -60.0f);
}

TEST_F(NeuronTestSuite, FiringState) {
    EXPECT_FALSE(neuron.isFiring());
    EXPECT_FALSE(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    EXPECT_TRUE(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    EXPECT_TRUE(neuron.isRefractory());
    EXPECT_FALSE(neuron.isFiring());
}

TEST_F(NeuronTestSuite, RefractoryDecrement) {
    neuron.setRefractoryPeriod(5);
    EXPECT_TRUE(neuron.isRefractory());
    
    neuron.decrementRefractory();
    EXPECT_TRUE(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    EXPECT_FALSE(neuron.isRefractory());
}

TEST_F(NeuronTestSuite, CurrentInjection) {
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
    
    neuron.injectCurrent(5.0f);
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
}

TEST_F(NeuronTestSuite, SynapticInput) {
    neuron.receiveExcitatoryInput(5.0f);
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), -3.0f);
}

TEST_F(NeuronTestSuite, SpikeHistory) {
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    EXPECT_EQ(neuron.getSpikeHistory().size(), 3);
    
    neuron.clearSpikeHistory();
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
}

TEST_F(NeuronTestSuite, RandomInitialization) {
    neuron.initializeRandom(rng);
    
    EXPECT_LT(neuron.getMembranePotential(), -60.0f);
    EXPECT_GT(neuron.getMembranePotential(), -80.0f);
    EXPECT_LT(neuron.getThreshold(), -50.0f);
    EXPECT_GT(neuron.getThreshold(), -60.0f);
}

TEST_F(NeuronTestSuite, Reset) {
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -70.0f);
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
}
