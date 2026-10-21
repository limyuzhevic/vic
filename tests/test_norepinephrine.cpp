// Norepinephrine Neuromodulator Tests
#include "neuromodulation/Norepinephrine.hpp"
#include <cassert>
#include <iostream>

namespace test_norepinephrine {

void testNorepinephrineCreation() {
    nlm::Norepinephrine norepinephrine;
    
    // Test basic properties
    assert(std::string(norepinephrine.getName()) == "NE");
    assert(norepinephrine.getLevel() >= 0.0f && norepinephrine.getLevel() <= 1.0f);
    
    std::cout << "    testNorepinephrineCreation passed" << std::endl;
}

void testNorepinephrineLevelControl() {
    nlm::Norepinephrine norepinephrine;
    
    // Test level setting
    norepinephrine.setLevel(0.5f);
    assert(norepinephrine.getLevel() == 0.5f);
    
    // Test clamping
    norepinephrine.setLevel(1.5f);
    assert(norepinephrine.getLevel() == 1.0f);
    
    norepinephrine.setLevel(-0.5f);
    assert(norepinephrine.getLevel() == 0.0f);
    
    std::cout << "    testNorepinephrineLevelControl passed" << std::endl;
}

void testNorepinephrineUpdate() {
    nlm::Norepinephrine norepinephrine;
    norepinephrine.setLevel(0.8f);
    
    // Update should decay towards baseline (0.0)
    float initialLevel = norepinephrine.getLevel();
    norepinephrine.update(1.0); // 1 second timestep
    assert(norepinephrine.getLevel() < initialLevel);
    
    std::cout << "    testNorepinephrineUpdate passed" << std::endl;
}

void testNorepinephrineStressSignaling() {
    nlm::Norepinephrine norepinephrine;
    
    float initialLevel = norepinephrine.getLevel();
    norepinephrine.signalStress(0.5f);
    assert(norepinephrine.getLevel() > initialLevel);
    
    std::cout << "    testNorepinephrineStressSignaling passed" << std::endl;
}

void testNorepinephrinePredictionErrorSignaling() {
    nlm::Norepinephrine norepinephrine;
    
    float initialLevel = norepinephrine.getLevel();
    norepinephrine.signalRewardPredictionError(0.3f);
    assert(norepinephrine.getLevel() > initialLevel);
    
    std::cout << "    testNorepinephrinePredictionErrorSignaling passed" << std::endl;
}

void testNorepinephrineNoveltySignaling() {
    nlm::Norepinephrine norepinephrine;
    
    float initialLevel = norepinephrine.getLevel();
    norepinephrine.signalNovelty();
    assert(norepinephrine.getLevel() > initialLevel);
    
    std::cout << "    testNorepinephrineNoveltySignaling passed" << std::endl;
}

void testNorepinephrineMemoryConsolidationSignaling() {
    nlm::Norepinephrine norepinephrine;
    
    float initialLevel = norepinephrine.getLevel();
    norepinephrine.signalMemoryConsolidation();
    assert(norepinephrine.getLevel() > initialLevel);
    
    std::cout << "    testNorepinephrineMemoryConsolidationSignaling passed" << std::endl;
}

void testNorepinephrinePlasticityFactor() {
    nlm::Norepinephrine norepinephrine;
    
    // Test plasticity factor calculation
    float plasticity = norepinephrine.getPlasticityFactor();
    assert(plasticity >= 0.6f && plasticity <= 1.0f);
    
    // Higher level should give higher plasticity factor
    norepinephrine.setLevel(0.9f);
    float higherPlasticity = norepinephrine.getPlasticityFactor();
    assert(higherPlasticity > plasticity);
    
    std::cout << "    testNorepinephrinePlasticityFactor passed" << std::endl;
}

void testNorepinephrineIntegrationWithBrain() {
    // Test that norepinephrine can be used in the brain context
    // This would require integration with the actual brain system
    
    nlm::Norepinephrine norepinephrine;
    
    // Basic integration test
    norepinephrine.setLevel(0.6f);
    float level = norepinephrine.getLevel();
    assert(level == 0.6f);
    
    std::cout << "    testNorepinephrineIntegrationWithBrain passed" << std::endl;
}

void testNorepinephrineArousalEffects() {
    nlm::Norepinephrine norepinephrine;
    
    // Test that stress signaling affects arousal
    norepinephrine.setLevel(0.2f);
    float initialArousal = norepinephrine.getLevel();
    norepinephrine.signalStress(0.4f);
    // Stress should increase norepinephrine level (arousal)
    assert(norepinephrine.getLevel() > initialArousal);
    
    std::cout << "    testNorepinephrineArousalEffects passed" << std::endl;
}

void testNorepinephrineAttentionEnhancement() {
    nlm::Norepinephrine norepinephrine;
    
    // Test that prediction error affects attention
    norepinephrine.setLevel(0.3f);
    float initialLevel = norepinephrine.getLevel();
    norepinephrine.signalRewardPredictionError(0.5f);
    // Prediction error should increase norepinephrine (attention enhancement)
    assert(norepinephrine.getLevel() > initialLevel);
    
    std::cout << "    testNorepinephrineAttentionEnhancement passed" << std::endl;
}

void runAll() {
    testNorepinephrineCreation();
    testNorepinephrineLevelControl();
    testNorepinephrineUpdate();
    testNorepinephrineStressSignaling();
    testNorepinephrinePredictionErrorSignaling();
    testNorepinephrineNoveltySignaling();
    testNorepinephrineMemoryConsolidationSignaling();
    testNorepinephrinePlasticityFactor();
    testNorepinephrineIntegrationWithBrain();
    testNorepinephrineArousalEffects();
    testNorepinephrineAttentionEnhancement();
}

} // namespace test_norepinephrine
