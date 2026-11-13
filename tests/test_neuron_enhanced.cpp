// Enhanced Neuron Tests with Test Framework

// Enhanced Neuron Tests with Test Framework

#include "tests/test_framework.h"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace test_neuron {

// Helper function to generate test names
template<typename T>
std::string generateTestName(const std::string& base, const T& value) {
    return base + "_" + std::to_string(value);
}

// Parameterized test for different neuron configurations
void runParameterizedNeuronTests() {
    std::vector<double> testCurrents = {0.0f, 1.0f, 5.0f, 10.0f};
    std::vector<double> membraneValues = {-70.0f, -65.0f, -60.0f, -55.0f};
    std::vector<int> refractoryPeriods = {0, 5, 10, 20};
    
    for (size_t i = 0; i < testCurrents.size(); ++i) {
        std::string testName = generateTestName("testNeuronCurrent", testCurrents[i]);
        RUN_TEST(testName, "NeuronProperties", [=]() {
            nlm::Neuron neuron(nlm::NeuronId(1));
            neuron.injectCurrent(testCurrents[i]);
            ASSERT_EQUAL(neuron.getTotalCurrent(), testCurrents[i], "Current injection failed");
        });
    }
    
    for (size_t i = 0; i < membraneValues.size(); ++i) {
        std::string testName = generateTestName("testMembraneSetting", membraneValues[i]);
        RUN_TEST(testName, "NeuronProperties", [=]() {
            nlm::Neuron neuron(nlm::NeuronId(1));
            neuron.setMembranePotential(membraneValues[i]);
            ASSERT_EQUAL(neuron.getMembranePotential(), membraneValues[i], "Membrane potential setting failed");
        });
    }
    
    for (size_t i = 0; i < refractoryPeriods.size(); ++i) {
        std::string testName = generateTestName("testRefractorySetting", refractoryPeriods[i]);
        RUN_TEST(testName, "NeuronProperties", [=]() {
            nlm::Neuron neuron(nlm::NeuronId(1));
            neuron.setRefractoryPeriod(refractoryPeriods[i]);
            ASSERT_EQUAL(neuron.isRefractory(), true, "Refractory period setting failed");
            for (int j = 0; j < refractoryPeriods[i]; ++j) {
                neuron.decrementRefractory();
            }
            ASSERT_EQUAL(neuron.isRefractory(), false, "Refractory period decrement failed");
        });
    }
}

// Test basic neuron creation
TEST_FUNCTION(testNeuronCreation) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT_EQUAL(neuron.getId(), nlm::NeuronId(1), "Neuron ID should be 1");
    ASSERT_EQUAL(neuron.getType(), nlm::NeuronType::Internal, "Default neuron type should be Internal");
}

// Test neuron type manipulation
TEST_FUNCTION(testNeuronType) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    ASSERT_EQUAL(neuron.getType(), nlm::NeuronType::Excitatory, "Type should be Excitatory");
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    ASSERT_EQUAL(neuron.getType(), nlm::NeuronType::Inhibitory, "Type should be Inhibitory");
    
    neuron.setType(nlm::NeuronType::Sensory);
    ASSERT_EQUAL(neuron.getType(), nlm::NeuronType::Sensory, "Type should be Sensory");
}

// Test membrane potential operations
TEST_FUNCTION(testMembranePotential) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test default value
    ASSERT_EQUAL(neuron.getMembranePotential(), -70.0f, "Default membrane potential should be -70.0mV");
    
    // Test setting
    neuron.setMembranePotential(-65.0f);
    ASSERT_EQUAL(neuron.getMembranePotential(), -65.0f, "Membrane potential should be set to -65.0mV");
    
    // Test addition
    neuron.addToMembranePotential(5.0f);
    ASSERT_EQUAL(neuron.getMembranePotential(), -60.0f, "Membrane potential should be -60.0mV after adding 5.0mV");
    
    // Test addition with negative value
    neuron.addToMembranePotential(-10.0f);
    ASSERT_EQUAL(neuron.getMembranePotential(), -70.0f, "Membrane potential should return to -70.0mV");
}

// Test firing state management
TEST_FUNCTION(testFiringState) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initial state
    ASSERT_FALSE(neuron.isFiring(), "Neuron should not be firing initially");
    ASSERT_FALSE(neuron.isRefractory(), "Neuron should not be refractory initially");
    
    // Set to active
    neuron.setFiringState(nlm::FiringState::Active);
    ASSERT_TRUE(neuron.isFiring(), "Neuron should be firing when in Active state");
    
    // Set refractory period
    neuron.setRefractoryPeriod(10);
    ASSERT_TRUE(neuron.isRefractory(), "Neuron should be refractory after setting period");
    ASSERT_FALSE(neuron.isFiring(), "Neuron should not be firing when refractory");
}

// Test refractory period decrement
TEST_FUNCTION(testRefractoryDecrement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    ASSERT_TRUE(neuron.isRefractory(), "Neuron should start as refractory");
    
    neuron.decrementRefractory();
    ASSERT_TRUE(neuron.isRefractory(), "Neuron should still be refractory after one decrement");
    
    // Decrement remaining periods
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    ASSERT_FALSE(neuron.isRefractory(), "Neuron should not be refractory after all decrements");
}

// Test current injection and clearing
TEST_FUNCTION(testCurrentInjection) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT_EQUAL(neuron.getTotalCurrent(), 0.0f, "Initial current should be 0.0f");
    
    neuron.injectCurrent(5.0f);
    ASSERT_EQUAL(neuron.getTotalCurrent(), 5.0f, "Injected current should be 5.0f");
    
    neuron.clearTotalCurrent();
    ASSERT_EQUAL(neuron.getTotalCurrent(), 0.0f, "Cleared current should be 0.0f");
}

// Test synaptic input handling
TEST_FUNCTION(testSynapticInput) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    ASSERT_EQUAL(neuron.getTotalCurrent(), 5.0f, "Excitatory input should add positive current");
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    ASSERT_EQUAL(neuron.getTotalCurrent(), -3.0f, "Inhibitory input should add negative current");
}

// Test spike history management
TEST_FUNCTION(testSpikeHistory) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    ASSERT_TRUE(neuron.getSpikeHistory().empty(), "Spike history should be empty initially");
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    ASSERT_EQUAL(neuron.getSpikeHistory().size(), 3, "Should have 3 spikes recorded");
    
    std::vector<float> history = neuron.getSpikeHistory();
    ASSERT_EQUAL(history[0], 0.1f, "First spike time should be 0.1");
    ASSERT_EQUAL(history[1], 0.2f, "Second spike time should be 0.2");
    ASSERT_EQUAL(history[2], 0.3f, "Third spike time should be 0.3");
    
    neuron.clearSpikeHistory();
    ASSERT_TRUE(neuron.getSpikeHistory().empty(), "Spike history should be empty after clearing");
}

// Test random initialization with seed
TEST_FUNCTION(testRandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Check that values are in reasonable ranges
    ASSERT_TRUE(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f, 
                "Membrane potential should be in reasonable range (-80 to -60 mV)");
    ASSERT_TRUE(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f,
                "Threshold should be in reasonable range (-60 to -50 mV)");
    
    // Test that same seed produces same results
    nlm::Neuron neuron2(nlm::NeuronId(2));
    neuron2.initializeRandom(rng);
    
    ASSERT_EQUAL(neuron.getMembranePotential(), neuron2.getMembranePotential(), 
                "Same seed should produce same membrane potential");
    ASSERT_EQUAL(neuron.getThreshold(), neuron2.getThreshold(), 
                "Same seed should produce same threshold");
}

// Test neuron reset functionality
TEST_FUNCTION(testReset) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Modify neuron state
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Reset neuron
    neuron.reset();
    
    // Check that state has been reset
    ASSERT_EQUAL(neuron.getMembranePotential(), -70.0f, "Membrane potential should reset to -70.0mV");
    ASSERT_EQUAL(neuron.getTotalCurrent(), 0.0f, "Current should be cleared");
    ASSERT_TRUE(neuron.getSpikeHistory().empty(), "Spike history should be cleared");
    ASSERT_FALSE(neuron.isFiring(), "Firing state should be reset to inactive");
}

// Main test runner
void runAll() {
    // Run basic tests
    RUN_TEST("testNeuronCreation", "NeuronProperties", []() { testNeuronCreation(); });
    RUN_TEST("testNeuronType", "NeuronProperties", []() { testNeuronType(); });
    RUN_TEST("testMembranePotential", "NeuronProperties", []() { testMembranePotential(); });
    RUN_TEST("testFiringState", "NeuronProperties", []() { testFiringState(); });
    RUN_TEST("testRefractoryDecrement", "NeuronProperties", []() { testRefractoryDecrement(); });
    RUN_TEST("testCurrentInjection", "NeuronProperties", []() { testCurrentInjection(); });
    RUN_TEST("testSynapticInput", "NeuronProperties", []() { testSynapticInput(); });
    RUN_TEST("testSpikeHistory", "NeuronProperties", []() { testSpikeHistory(); });
    RUN_TEST("testRandomInitialization", "NeuronProperties", []() { testRandomInitialization(); });
    RUN_TEST("testReset", "NeuronProperties", []() { testReset(); });
    
    // Run parameterized tests
    runParameterizedNeuronTests();
}

} // namespace test_neuron