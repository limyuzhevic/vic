// Neuron Tests
// Enhanced with NLM Test Framework
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <string>

// Test macros for NLM Test Framework
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

namespace test_neuron {

// Enhanced test functions with modern assertions
void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    NLM_ASSERT_EQ(neuron.getId(), nlm::NeuronId(1));
    NLM_ASSERT_EQ(neuron.getType(), nlm::NeuronType::Internal);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    NLM_ASSERT_EQ(neuron.getType(), nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    NLM_ASSERT_EQ(neuron.getType(), nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    NLM_ASSERT_EQ(neuron.getMembranePotential(), -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    NLM_ASSERT_EQ(neuron.getMembranePotential(), -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    NLM_ASSERT_EQ(neuron.getMembranePotential(), -60.0f);
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    NLM_ASSERT_FALSE(neuron.isFiring());
    NLM_ASSERT_FALSE(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    NLM_ASSERT_TRUE(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    NLM_ASSERT_TRUE(neuron.isRefractory());
    NLM_ASSERT_FALSE(neuron.isFiring());
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    NLM_ASSERT_TRUE(neuron.isRefractory());
    
    neuron.decrementRefractory();
    NLM_ASSERT_TRUE(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    NLM_ASSERT_FALSE(neuron.isRefractory());
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    NLM_ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
    
    neuron.injectCurrent(5.0f);
    NLM_ASSERT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    NLM_ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    NLM_ASSERT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    NLM_ASSERT_EQ(neuron.getTotalCurrent(), -3.0f);
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    NLM_ASSERT_TRUE(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    NLM_ASSERT_EQ(neuron.getSpikeHistory().size(), 3);
    
    neuron.clearSpikeHistory();
    NLM_ASSERT_TRUE(neuron.getSpikeHistory().empty());
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

void testRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    NLM_ASSERT_TRUE(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    NLM_ASSERT_TRUE(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    NLM_ASSERT_EQ(neuron.getMembranePotential(), -70.0f);
    NLM_ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
    NLM_ASSERT_TRUE(neuron.getSpikeHistory().empty());
    
    std::cout << "    testReset passed" << std::endl;
}

// Modern test framework integration
void testNeuronAdvancedFeatures() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test advanced features with modern assertions
    neuron.setType(nlm::NeuronType::Excitatory);
    NLM_ASSERT_TRUE(neuron.isExcitatory());
    NLM_ASSERT_FALSE(neuron.isInhibitory());
    
    neuron.setMembranePotential(-65.0f);
    NLM_ASSERT_GT(neuron.getMembranePotential(), -70.0f);
    NLM_ASSERT_LE(neuron.getMembranePotential(), -60.0f);
    
    std::cout << "    testNeuronAdvancedFeatures passed" << std::endl;
}

void testNeuronPlasticity() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test plasticity features
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setRefractoryPeriod(5);
    
    NLM_ASSERT_TRUE(neuron.getPlasticityFlags().hasPlasticity);
    NLM_ASSERT_TRUE(neuron.getPlasticityFlags().hasSTDP);
    NLM_ASSERT_TRUE(neuron.getPlasticityFlags().hasRewardModulation);
    
    std::cout << "    testNeuronPlasticity passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Neuron tests..." << std::endl;
    
    testNeuronCreation();
    testNeuronType();
    testMembranePotential();
    testFiringState();
    testRefractoryDecrement();
    testCurrentInjection();
    testSynapticInput();
    testSpikeHistory();
    testRandomInitialization();
    testReset();
    testNeuronAdvancedFeatures();
    testNeuronPlasticity();
}

} // namespace test_neuron
