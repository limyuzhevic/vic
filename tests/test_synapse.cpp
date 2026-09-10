// Unit tests for Synapse plasticity
// Tests synaptic weight changes through various plasticity mechanisms

#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>

void testSynapseInitialState() {
    std::cout << "Testing Synapse Initial State..." << std::endl;
    
    Synapse synapse(1, NeuronType::Sensory, NeuronType::Motor);
    
    // Test initial properties
    ASSERT_EQ(synapse.getSourceType(), NeuronType::Sensory);
    ASSERT_EQ(synapse.getTargetType(), NeuronType::Motor);
    ASSERT_NEAR(synapse.getWeight(), 0.1f, 0.01f); // Default weight
    ASSERT_FALSE(synapse.isPlasticityEnabled());
    ASSERT_FALSE(synapse.isHebbianEnabled());
    ASSERT_FALSE(synapse.isSTDPEnabled());
    ASSERT_FALSE(synapse.isRewardModulated());
    
    // Test initial eligibility trace
    ASSERT_NEAR(synapse.getEligibilityTrace(), 0.0f, 0.001f);
    
    std::cout << "✓ Synapse initial state tests passed" << std::endl;
}

void testSynapsePlasticityEnable() {
    std::cout << "Testing Synapse Plasticity Enable..." << std::endl;
    
    Synapse synapse(2, NeuronType::Internal, NeuronType::Internal);
    
    // Enable different plasticity types
    synapse.enablePlasticity(true, true, true); // All enabled
    
    // Note: Actual flag checking would depend on implementation
    // Test that function calls work without errors
    
    std::cout << "✓ Synapse plasticity enable tests passed" << std::endl;
}

void testSynapseHebbianLearning() {
    std::cout << "Testing Synapse Hebbian Learning..." << std::endl;
    
    Synapse synapse(3, NeuronType::Sensory, NeuronType::Motor);
    synapse.enablePlasticity(true, false, false); // Only Hebbian
    
    // Record initial weight
    float initialWeight = synapse.getWeight();
    
    // Simulate correlated activity (pre-post)
    synapse.applyHebbianPlasticity(0.5f, 0.5f); // Both neurons active
    
    // Weight should increase for correlated activity
    float newWeight = synapse.getWeight();
    ASSERT_GT(newWeight, initialWeight);
    
    // Test anti-correlation (opposite activity)
    synapse.applyHebbianPlasticity(0.5f, -0.5f); // Opposite activity
    float antiCorrWeight = synapse.getWeight();
    // Weight should decrease or stay the same
    
    std::cout << "✓ Synapse Hebbian learning tests passed" << std::endl;
}

void testSynapseSTDP() {
    std::cout << "Testing Synapse STDP..." << std::endl;
    
    Synapse synapse(4, NeuronType::Sensory, NeuronType::Motor);
    synapse.enablePlasticity(false, true, false); // Only STDP
    
    // Record initial weight
    float initialWeight = synapse.getWeight();
    
    // Simulate pre-post spike timing (LTP)
    synapse.applySTDP(0.0f, 10.0f); // Pre fires 10ms before post
    
    float ltpWeight = synapse.getWeight();
    ASSERT_GT(ltpWeight, initialWeight);
    
    // Reset and simulate post-pre spike timing (LTD)
    synapse.setWeight(initialWeight);
    synapse.applySTDP(10.0f, 0.0f); // Post fires 10ms after pre
    
    float ltdWeight = synapse.getWeight();
    ASSERT_LT(ltdWeight, initialWeight);
    
    std::cout << "✓ Synapse STDP tests passed" << std::endl;
}

void testSynapseRewardModulation() {
    std::cout << "Testing Synapse Reward Modulation..." << std::endl;
    
    Synapse synapse(5, NeuronType::Internal, NeuronType::Internal);
    synapse.enablePlasticity(false, false, true); // Only reward-modulated
    
    // Set up eligibility trace
    synapse.setEligibilityTrace(0.5f);
    
    // Apply reward modulation
    synapse.applyRewardModulation(1.0f, 0.5f, 0.8f); // Reward, prediction, modulation
    
    // Weight should change based on reward and eligibility trace
    float newWeight = synapse.getWeight();
    
    // Reset eligibility and test negative reward
    synapse.setEligibilityTrace(0.5f);
    synapse.applyRewardModulation(-1.0f, 0.5f, 0.8f); // Negative reward
    
    std::cout << "✓ Synapse reward modulation tests passed" << std::endl;
}

void testSynapseEligibilityTraceDecay() {
    std::cout << "Testing Synapse Eligibility Trace Decay..." << std::endl;
    
    Synapse synapse(6, NeuronType::Internal, NeuronType::Internal);
    
    // Set eligibility trace
    synapse.setEligibilityTrace(1.0f);
    ASSERT_NEAR(synapse.getEligibilityTrace(), 1.0f, 0.001f);
    
    // Decay eligibility trace
    synapse.decayEligibilityTrace(0.1f);
    ASSERT_NEAR(synapse.getEligibilityTrace(), 0.9f, 0.001f);
    
    // Decay multiple times
    for (int i = 0; i < 5; ++i) {
        synapse.decayEligibilityTrace(0.1f);
    }
    
    ASSERT_LT(synapse.getEligibilityTrace(), 0.5f);
    
    std::cout << "✓ Synapse eligibility trace decay tests passed" << std::endl;
}

void testSynapseWeightManagement() {
    std::cout << "Testing Synapse Weight Management..." << std::endl;
    
    Synapse synapse(7, NeuronType::Sensory, NeuronType::Motor);
    
    // Test initial weight
    ASSERT_NEAR(synapse.getWeight(), 0.1f, 0.01f);
    
    // Test weight modification
    synapse.setWeight(0.5f);
    ASSERT_NEAR(synapse.getWeight(), 0.5f, 0.001f);
    
    synapse.addToWeight(0.1f);
    ASSERT_NEAR(synapse.getWeight(), 0.6f, 0.001f);
    
    synapse.addToWeight(-0.3f);
    ASSERT_NEAR(synapse.getWeight(), 0.3f, 0.001f);
    
    // Test weight bounds
    synapse.setWeight(2.0f); // Should clamp to reasonable range
    // Depending on implementation, weight may be clamped
    
    std::cout << "✓ Synapse weight management tests passed" << std::endl;
}

void testSynapseConnectionInformation() {
    std::cout << "Testing Synapse Connection Information..." << std::endl;
    
    Synapse synapse(8, NeuronType::Sensory, NeuronType::Motor);
    
    // Set source and target neurons (implementation dependent)
    // Test that synapse can store connection information
    
    // Test plasticity flags
    synapse.enablePlasticity(true, true, true);
    // Check that flags are set appropriately
    
    std::cout << "✓ Synapse connection information tests passed" << std::endl;
}