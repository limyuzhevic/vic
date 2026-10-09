// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "nlm/test_framework.h"
#include <iostream>

namespace test_neuron {

TEST_CASE(testNeuronCreation) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT_EQ(neuron.getId(), nlm::NeuronId(1));
    ASSERT_EQ(neuron.getType(), nlm::NeuronType::Internal);
}

TEST_CASE(testNeuronType) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    ASSERT_EQ(neuron.getType(), nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    ASSERT_EQ(neuron.getType(), nlm::NeuronType::Inhibitory);
}

TEST_CASE(testMembranePotential) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT_EQ(neuron.getMembranePotential(), -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    ASSERT_EQ(neuron.getMembranePotential(), -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    ASSERT_EQ(neuron.getMembranePotential(), -60.0f);
}

TEST_CASE(testFiringState) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT_FALSE(neuron.isFiring());
    ASSERT_FALSE(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    ASSERT_TRUE(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    ASSERT_TRUE(neuron.isRefractory());
    ASSERT_FALSE(neuron.isFiring());
}

TEST_CASE(testRefractoryDecrement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    ASSERT_TRUE(neuron.isRefractory());
    
    neuron.decrementRefractory();
    ASSERT_TRUE(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    ASSERT_FALSE(neuron.isRefractory());
}

TEST_CASE(testCurrentInjection) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
    
    neuron.injectCurrent(5.0f);
    ASSERT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
}

TEST_CASE(testSynapticInput) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    ASSERT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    ASSERT_EQ(neuron.getTotalCurrent(), -3.0f);
}

TEST_CASE(testSpikeHistory) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT_TRUE(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    ASSERT_EQ(neuron.getSpikeHistory().size(), 3);
    
    neuron.clearSpikeHistory();
    ASSERT_TRUE(neuron.getSpikeHistory().empty());
}

TEST_CASE(testRandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    ASSERT_TRUE(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    ASSERT_TRUE(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
}

TEST_CASE(testReset) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    ASSERT_EQ(neuron.getMembranePotential(), -70.0f);
    ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
    ASSERT_TRUE(neuron.getSpikeHistory().empty());
}

std::vector<TestResult> runAllTests() {
    return testNeuron::runAllTests();
}

} // namespace test_neuron

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_neuron::runAllTests();
    printTestSummary(results);
}
