// Dopamine Neuromodulation Tests
// Tests for reward prediction error signaling and dopaminergic learning

#include "neuromodulation/Dopamine.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_dopamine {

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "DA");
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineLevel() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getLevel() == 0.0f);
    
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    // Test bounds
    dopamine.setLevel(2.0f);
    assert(dopamine.getLevel() == 1.0f);  // Clamped to max
    
    dopamine.setLevel(-0.5f);
    assert(dopamine.getLevel() == 0.0f);  // Clamped to min
    
    std::cout << "    testDopamineLevel passed" << std::endl;
}

void testDopaminePlasticityFactor() {
    nlm::Dopamine dopamine;
    
    // Test plasticity factor (placeholder implementation)
    float factor = dopamine.getPlasticityFactor();
    assert(factor >= 0.5f && factor <= 1.0f);  // Higher dopamine = higher plasticity
    
    // Increase dopamine level
    dopamine.setLevel(1.0f);
    float highFactor = dopamine.getPlasticityFactor();
    
    assert(highFactor > factor);
    assert(highFactor <= 1.0f);
    
    std::cout << "    testDopaminePlasticityFactor passed" << std::endl;
}

void testDopamineUpdate() {
    nlm::Dopamine dopamine;
    
    // Set level above baseline
    dopamine.setLevel(0.5f);
    
    // Initial update should decay towards baseline
    dopamine.update(0.1f);  // 100ms
    assert(dopamine.getLevel() < 0.5f);
    
    // Test decay rate
    float levelAfterUpdate = dopamine.getLevel();
    dopamine.update(0.1f);
    assert(dopamine.getLevel() <= levelAfterUpdate);
    
    std::cout << "    testDopamineUpdate passed" << std::endl;
}

void testDopamineRewardSignaling() {
    nlm::Dopamine dopamine;
    
    // Signal reward - should increase dopamine level
    float initialLevel = dopamine.getLevel();
    
    dopamine.signalReward(0.5f);
    float afterReward = dopamine.getLevel();
    
    assert(afterReward > initialLevel);
    
    // Signal maximum reward - should clamp to peak
    float peak = 1.0f;  // Assumed peak from implementation
    dopamine.signalReward(2.0f);
    assert(dopamine.getLevel() <= peak);
    
    std::cout << "    testDopamineRewardSignaling passed" << std::endl;
}

void testDopaminePredictionError() {
    nlm::Dopamine dopamine;
    
    float initialLevel = dopamine.getLevel();
    
    // Positive prediction error (better than expected)
    dopamine.signalRewardPredictionError(0.5f);
    assert(dopamine.getLevel() > initialLevel);
    
    // Negative prediction error (worse than expected)
    float levelBeforeNegative = dopamine.getLevel();
    dopamine.signalRewardPredictionError(-0.5f);
    assert(dopamine.getLevel() >= 0.0f);  // Can't go below baseline
    
    std::cout << "    testDopaminePredictionError passed" << std::endl;
}

void testDopamineIntegration() {
    nlm::Dopamine dopamine;
    
    // Test integration with other plasticity systems
    // Higher dopamine should increase plasticity factor
    float baselineFactor = dopamine.getPlasticityFactor();
    
    dopamine.setLevel(1.0f);  // Max dopamine
    float highDopamineFactor = dopamine.getPlasticityFactor();
    
    assert(highDopamineFactor > baselineFactor);
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Dopamine tests..." << std::endl;
    testDopamineCreation();
    testDopamineLevel();
    testDopaminePlasticityFactor();
    testDopamineUpdate();
    testDopamineRewardSignaling();
    testDopaminePredictionError();
    testDopamineIntegration();
}

} // namespace test_dopamine