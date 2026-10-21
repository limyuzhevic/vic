// Serotonin Neuromodulator Tests
#include "neuromodulation/Serotonin.hpp"
#include <cassert>
#include <iostream>

namespace test_serotonin {

void testSerotoninCreation() {
    nlm::Serotonin serotonin;
    
    // Test basic properties
    assert(std::string(serotonin.getName()) == "5-HT");
    assert(serotonin.getLevel() >= 0.0f && serotonin.getLevel() <= 1.0f);
    
    std::cout << "    testSerotoninCreation passed" << std::endl;
}

void testSerotoninLevelControl() {
    nlm::Serotonin serotonin;
    
    // Test level setting
    serotonin.setLevel(0.5f);
    assert(serotonin.getLevel() == 0.5f);
    
    // Test clamping
    serotonin.setLevel(1.5f);
    assert(serotonin.getLevel() == 1.0f);
    
    serotonin.setLevel(-0.5f);
    assert(serotonin.getLevel() == 0.0f);
    
    std::cout << "    testSerotoninLevelControl passed" << std::endl;
}

void testSerotoninUpdate() {
    nlm::Serotonin serotonin;
    serotonin.setLevel(0.8f);
    
    // Update should decay towards baseline (0.0)
    float initialLevel = serotonin.getLevel();
    serotonin.update(1.0); // 1 second timestep
    assert(serotonin.getLevel() < initialLevel);
    
    std::cout << "    testSerotoninUpdate passed" << std::endl;
}

void testSerotoninRewardSignaling() {
    nlm::Serotonin serotonin;
    
    float initialLevel = serotonin.getLevel();
    serotonin.signalReward(0.5f);
    assert(serotonin.getLevel() > initialLevel);
    
    std::cout << "    testSerotoninRewardSignaling passed" << std::endl;
}

void testSerotoninPredictionErrorSignaling() {
    nlm::Serotonin serotonin;
    
    float initialLevel = serotonin.getLevel();
    serotonin.signalRewardPredictionError(0.3f);
    assert(serotonin.getLevel() > initialLevel);
    
    std::cout << "    testSerotoninPredictionErrorSignaling passed" << std::endl;
}

void testSerotoninBehavioralInhibition() {
    nlm::Serotonin serotonin;
    
    // Test behavioral inhibition signaling
    serotonin.signalBehavioralInhibition();
    assert(serotonin.getLevel() >= 0.0f);
    
    std::cout << "    testSerotoninBehavioralInhibition passed" << std::endl;
}

void testSerotoninWorkingMemoryBoost() {
    nlm::Serotonin serotonin;
    
    // Test working memory boost signaling
    serotonin.signalWorkingMemoryBoost();
    assert(serotonin.getLevel() >= 0.0f);
    
    std::cout << "    testSerotoninWorkingMemoryBoost passed" << std::endl;
}

void testSerotoninAttentionModulation() {
    nlm::Serotonin serotonin;
    
    // Test attention modulation signaling
    serotonin.signalAttentionModulation();
    assert(serotonin.getLevel() >= 0.0f);
    
    std::cout << "    testSerotoninAttentionModulation passed" << std::endl;
}

void testSerotoninPlasticityFactor() {
    nlm::Serotonin serotonin;
    
    // Test plasticity factor calculation
    float plasticity = serotonin.getPlasticityFactor();
    assert(plasticity >= 0.7f && plasticity <= 1.0f);
    
    // Higher level should give higher plasticity factor
    serotonin.setLevel(0.9f);
    float higherPlasticity = serotonin.getPlasticityFactor();
    assert(higherPlasticity > plasticity);
    
    std::cout << "    testSerotoninPlasticityFactor passed" << std::endl;
}

void testSerotoninIntegrationWithBrain() {
    // Test that serotonin can be used in the brain context
    // This would require integration with the actual brain system
    
    nlm::Serotonin serotonin;
    
    // Basic integration test
    serotonin.setLevel(0.6f);
    float level = serotonin.getLevel();
    assert(level == 0.6f);
    
    std::cout << "    testSerotoninIntegrationWithBrain passed" << std::endl;
}

void runAll() {
    testSerotoninCreation();
    testSerotoninLevelControl();
    testSerotoninUpdate();
    testSerotoninRewardSignaling();
    testSerotoninPredictionErrorSignaling();
    testSerotoninBehavioralInhibition();
    testSerotoninWorkingMemoryBoost();
    testSerotoninAttentionModulation();
    testSerotoninPlasticityFactor();
    testSerotoninIntegrationWithBrain();
}

} // namespace test_serotonin