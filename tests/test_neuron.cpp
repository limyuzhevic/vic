// Neuron Tests - GoogleTest Version
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <cmath>

namespace {

TEST(NeuronTest, NeuronCreation) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_EQ(neuron.getId().index(), 1);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Internal);
}

TEST(NeuronTest, NeuronType) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Inhibitory);
}

TEST(NeuronTest, MembranePotential) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -70.0);
    
    neuron.setMembranePotential(-65.0);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -65.0);
    
    neuron.addToMembranePotential(5.0);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -60.0);
}

TEST(NeuronTest, FiringState) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_FALSE(neuron.isFiring());
    EXPECT_FALSE(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    EXPECT_TRUE(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    EXPECT_TRUE(neuron.isRefractory());
    EXPECT_FALSE(neuron.isFiring());
}

TEST(NeuronTest, RefractoryDecrement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    EXPECT_TRUE(neuron.isRefractory());
    
    neuron.decrementRefractory();
    EXPECT_TRUE(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    EXPECT_FALSE(neuron.isRefractory());
}

TEST(NeuronTest, CurrentInjection) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), 0.0);
    
    neuron.injectCurrent(5.0);
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), 5.0);
    
    neuron.clearTotalCurrent();
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), 0.0);
}

TEST(NeuronTest, SynapticInput) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0);
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), 5.0);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0);
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), -3.0);
}

TEST(NeuronTest, SpikeHistory) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    auto history = neuron.getSpikeHistory();
    ASSERT_EQ(history.size(), 3);
    EXPECT_DOUBLE_EQ(history[0], 0.1);
    EXPECT_DOUBLE_EQ(history[1], 0.2);
    EXPECT_DOUBLE_EQ(history[2], 0.3);
    
    neuron.clearSpikeHistory();
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
}

TEST(NeuronTest, RandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    float membranePotential = neuron.getMembranePotential();
    float threshold = neuron.getThreshold();
    
    EXPECT_LT(membranePotential, -60.0f);
    EXPECT_GT(membranePotential, -80.0f);
    EXPECT_LT(threshold, -50.0f);
    EXPECT_GT(threshold, -60.0f);
}

TEST(NeuronTest, Reset) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0);
    neuron.injectCurrent(5.0);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -70.0);
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), 0.0);
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
}

TEST(NeuronTest, StateTransitions) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test state transitions
    neuron.setFiringState(nlm::FiringState::Active);
    EXPECT_TRUE(neuron.isFiring());
    
    neuron.setFiringState(nlm::FiringState::Silent);
    EXPECT_FALSE(neuron.isFiring());
    
    neuron.setRefractoryPeriod(1);
    EXPECT_TRUE(neuron.isRefractory());
}

TEST(NeuronTest, MembranePotentialBoundaries) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test that membrane potential can be set within reasonable bounds
    neuron.setMembranePotential(-90.0);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -90.0);
    
    neuron.setMembranePotential(-30.0);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -30.0);
    
    // Test that we can increment/decrement safely
    neuron.addToMembranePotential(-10.0);
    float newPotential = neuron.getMembranePotential();
    EXPECT_LT(newPotential, -30.0);
}

} // namespace

// Backward compatibility - keep the runAll function for existing test_main.cpp
namespace test_neuron {
    void runAll();
}