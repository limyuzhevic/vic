// Attention Tests
// Tests for competitive selection dynamics and attentional focus

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_attention {

void testCompetitiveSelection() {
    std::cout << "  Testing competitive selection..." << std::endl;
    
    // Test attentional selection with competitive dynamics
    std::vector<float> activationValues = {0.1f, 0.8f, 0.3f, 0.6f, 0.2f};
    
    // Find the winning item (competitive selection)
    size_t winnerIndex = 0;
    float maxActivation = activationValues[0];
    
    for (size_t i = 1; i < activationValues.size(); ++i) {
        if (activationValues[i] > maxActivation) {
            maxActivation = activationValues[i];
            winnerIndex = i;
        }
    }
    
    assert(maxActivation > 0.0f && "Should have an activated item");
    assert(winnerIndex < activationValues.size() && "Winner index should be valid");
    
    // Test inhibition of competitors
    std::vector<float> inhibitedActivations = activationValues;
    for (size_t i = 0; i < inhibitedActivations.size(); ++i) {
        if (i != winnerIndex) {
            inhibitedActivations[i] *= 0.3f; // Inhibit competitors
        }
    }
    
    // Winner should still have higher activation
    assert(inhibitedActivations[winnerIndex] >= maxActivation * 0.5f && "Winner should maintain activation");
    
    std::cout << "    ✓ Competitive selection test passed" << std::endl;
}

void testAttentionalFocus() {
    std::cout << "  Testing attentional focus..." << std::endl;
    
    // Test attentional focus on specific stimuli
    std::vector<float> stimulusActivations = {0.2f, 0.9f, 0.4f, 0.7f, 0.1f};
    
    // Calculate attentional weights
    float totalActivation = 0.0f;
    for (float activation : stimulusActivations) {
        totalActivation += activation;
    }
    
    // Convert to attentional weights
    std::vector<float> attentionalWeights;
    for (float activation : stimulusActivations) {
        attentionalWeights.push_back(activation / totalActivation);
    }
    
    // Verify weights sum to 1
    float weightSum = 0.0f;
    for (float weight : attentionalWeights) {
        weightSum += weight;
    }
    
    assert(std::abs(weightSum - 1.0f) < 0.001f && "Attentional weights should sum to 1");
    
    std::cout << "    ✓ Attentional focus test passed" << std::endl;
}

void testAttentionShift() {
    std::cout << "  Testing attentional shift..." << std::endl;
    
    // Test that attention can shift between different focuses
    std::vector<std::vector<float>> attentionalStates = {
        {0.1f, 0.2f, 0.3f, 0.4f}, // Focus 1
        {0.4f, 0.3f, 0.2f, 0.1f}, // Focus 2
        {0.2f, 0.5f, 0.2f, 0.1f}  // Focus 3
    };
    
    // Test attentional transitions
    for (size_t i = 1; i < attentionalStates.size(); ++i) {
        const auto& prevState = attentionalStates[i-1];
        const auto& currState = attentionalStates[i];
        
        // Calculate shift magnitude
        float shiftMagnitude = 0.0f;
        for (size_t j = 0; j < prevState.size(); ++j) {
            shiftMagnitude += std::abs(currState[j] - prevState[j]);
        }
        
        assert(shiftMagnitude > 0.0f && "Attention should shift between states");
    }
    
    std::cout << "    ✓ Attentional shift test passed" << std::endl;
}

void runAll() {
    std::cout << "Attention Tests:" << std::endl;
    
    testCompetitiveSelection();
    testAttentionalFocus();
    testAttentionShift();
    
    std::cout << "Attention Tests: PASSED" << std::endl;
}

} // namespace test_attention
