// Comprehensive Neuron Unit Tests
// File: test_neuron_comprehensive.cpp
// Coverage: Neuron.cpp, Neuron.hpp

#include <gtest/gtest.h>
#include "brain/Neuron.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

class NeuronComprehensiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for each test
        neuron = std::make_unique<Neuron>(NeuronId(1));
        neuron->setType(NeuronType::Excitatory);
        neuron->setThreshold(-50.0f);
        neuron->setLeakConductance(10.0f);
    }
    
    void TearDown() override {
        // Cleanup
        neuron.reset();
    }
    
    std::unique_ptr<Neuron> neuron;
};

TEST_F(NeuronComprehensiveTest, BasicCreation) {
    // Test neuron creation and basic properties
    EXPECT_EQ(neuron->getId(), NeuronId(1));
    EXPECT_EQ(neuron->getType(), NeuronType::Excitatory);
    EXPECT_TRUE(neuron->isFiring());
    EXPECT_FALSE(neuron->isRefractory());
}

TEST_F(NeuronComprehensiveTest, MembranePotentialDynamics) {
    // Test LIF neuron membrane potential dynamics
    float initialPot = neuron->getMembranePotential();
    
    // Test setting membrane potential
    neuron->setMembranePotential(-60.0f);
    EXPECT_EQ(neuron->getMembranePotential(), -60.0f);
    
    // Test adding to membrane potential
    neuron->addToMembranePotential(15.0f);
    EXPECT_EQ(neuron->getMembranePotential(), -45.0f);
}

TEST_F(NeuronComprehensiveTest, LIFThresholdChecking) {
    // Test Leaky Integrate-and-Fire threshold detection
    neuron->setMembranePotential(-55.0f);
    neuron->setThreshold(-50.0f);
    
    // Below threshold
    neuron->setMembranePotential(-60.0f);
    EXPECT_FALSE(neuron->checkThreshold());
    
    // At threshold
    neuron->setMembranePotential(-50.0f);
    EXPECT_TRUE(neuron->checkThreshold());
    
    // Above threshold
    neuron->setMembranePotential(-40.0f);
    EXPECT_TRUE(neuron->checkThreshold());
}

TEST_F(NeuronComprehensiveTest, RefractoryPeriod) {
    // Test refractory period functionality
    neuron->setRefractoryPeriod(10);
    neuron->setFiringState(FiringState::Refractory);
    
    EXPECT_TRUE(neuron->isRefractory());
    EXPECT_FALSE(neuron->isFiring());
    
    // Test refractory countdown
    for (int i = 0; i < 5; ++i) {
        neuron->decrementRefractory();
    }
    EXPECT_TRUE(neuron->isRefractory());
    
    for (int i = 5; i < 10; ++i) {
        neuron->decrementRefractory();
    }
    EXPECT_FALSE(neuron->isRefractory());
    EXPECT_EQ(neuron->getState().firingState, FiringState::Resting);
}

TEST_F(NeuronComprehensiveTest, FiringRateControl) {
    // Test firing rate control
    neuron->setFiringRate(10.0f);
    EXPECT_EQ(neuron->getFiringRate(), 10.0f);
    
    neuron->setFiringRate(50.5f);
    EXPECT_EQ(neuron->getFiringRate(), 50.5f);
}

TEST_F(NeuronComprehensiveTest, LeakConductance) {
    // Test leak conductance parameter
    neuron->setLeakConductance(5.0f);
    EXPECT_EQ(neuron->getLeakConductance(), 5.0f);
    
    neuron->setLeakConductance(20.0f);
    EXPECT_EQ(neuron->getLeakConductance(), 20.0f);
}

TEST_F(NeuronComprehensiveTest, RestingAndResetPotential) {
    // Test resting and reset potential
    neuron->setRestingPotential(-75.0f);
    EXPECT_EQ(neuron->getRestingPotential(), -75.0f);
    
    neuron->setResetPotential(-65.0f);
    EXPECT_EQ(neuron->getResetPotential(), -65.0f);
}

TEST_F(NeuronComprehensiveTest, SpikeHistory) {
    // Test spike history management
    // Add some spike times
    neuron->recordSpike(100.0f);
    neuron->recordSpike(200.0f);
    
    EXPECT_EQ(neuron->getLastSpikeTime(), 200.0f);
    EXPECT_GT(neuron->getSpikeCount(), 0);
}

TEST_F(NeuronComprehensiveTest, SynapticInputIntegration) {
    // Test synaptic input integration
    neuron->addSynapticInput(10.5f);
    neuron->addSynapticInput(-5.2f);
    
    EXPECT_EQ(neuron->getState().synapseConductance, 5.3f);
}

TEST_F(NeuronComprehensiveTest, RefractoryPeriodBoundaryCases) {
    // Test edge cases for refractory period
    neuron->setRefractoryPeriod(0);
    neuron->setFiringState(FiringState::Refractory);
    neuron->decrementRefractory();
    EXPECT_FALSE(neuron->isRefractory());
}

TEST_F(NeuronComprehensiveTest, MembranePotentialBounds) {
    // Test membrane potential boundary conditions
    neuron->setMembranePotential(-100.0f);
    EXPECT_GE(neuron->getMembranePotential(), -100.0f);
    
    neuron->setMembranePotential(50.0f);
    EXPECT_LE(neuron->getMembranePotential(), 50.0f);
}

TEST_F(NeuronComprehensiveTest, StateConsistency) {
    // Test state consistency after multiple operations
    float initialPot = neuron->getMembranePotential();
    neuron->addToMembranePotential(10.0f);
    neuron->addToMembranePotential(-5.0f);
    
    EXPECT_NEAR(neuron->getMembranePotential(), initialPot + 5.0f, 0.001f);
}

TEST_F(NeuronComprehensiveTest, NeuronTypeChanges) {
    // Test neuron type transitions
    EXPECT_EQ(neuron->getType(), NeuronType::Excitatory);
    
    neuron->setType(NeuronType::Inhibitory);
    EXPECT_EQ(neuron->getType(), NeuronType::Inhibitory);
    
    neuron->setType(NeuronType::Internal);
    EXPECT_EQ(neuron->getType(), NeuronType::Internal);
}

TEST_F(NeuronComprehensiveTest, FiringStateTransitions) {
    // Test firing state transitions
    neuron->setFiringState(FiringState::Active);
    EXPECT_TRUE(neuron->isFiring());
    
    neuron->setFiringState(FiringState::Resting);
    EXPECT_FALSE(neuron->isFiring());
    
    neuron->setFiringState(FiringState::Refractory);
    EXPECT_TRUE(neuron->isRefractory());
}

} // namespace nlm

