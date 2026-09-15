// Neuromodulation integration tests
// Tests for ACh, NE, 5-HT, and Reward neuromodulators

#include <cassert>
#include <iostream>
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Reward.hpp"

namespace nlm {
    
    void testAcetylcholine() {
        std::cout << "Testing Acetylcholine..." << std::endl;
        
        Acetylcholine ach;
        
        // Test initialization
        assert(ach.getName() != nullptr);
        assert(std::string(ach.getName()) == "ACh");
        
        // Test default level
        assert(ach.getLevel() == 0.0f);
        
        // Test level setting
        ach.setLevel(0.5f);
        assert(ach.getLevel() == 0.5f);
        
        // Test level clamping
        ach.setLevel(1.5f);
        assert(ach.getLevel() == 1.0f);
        
        ach.setLevel(-0.5f);
        assert(ach.getLevel() == 0.0f);
        
        // Test plasticity factor
        float plasticity = ach.getPlasticityFactor();
        assert(plasticity >= 0.5f && plasticity <= 1.0f);
        
        // Test attention signaling
        ach.signalAttention(0.3f);
        assert(ach.getLevel() > 0.0f);
        
        ach.signalMemoryEvent(0.4f);
        assert(ach.getLevel() > 0.0f);
        
        // Test attention modulation
        ach.updateAttentionModulation();
        float attentionMod = ach.getAttentionModulation();
        assert(attentionMod >= 1.0f && attentionMod <= 3.0f);
        
        std::cout << "✓ Acetylcholine tests passed" << std::endl;
    }
    
    void testNorepinephrine() {
        std::cout << "Testing Norepinephrine..." << std::endl;
        
        Norepinephrine ne;
        
        // Test initialization
        assert(ne.getName() != nullptr);
        assert(std::string(ne.getName()) == "NE");
        
        // Test default level
        assert(ne.getLevel() == 0.0f);
        
        // Test level setting
        ne.setLevel(0.8f);
        assert(ne.getLevel() == 0.8f);
        
        // Test sensory gain
        ne.updateSensoryGain();
        float sensoryGain = ne.getSensoryGain();
        assert(sensoryGain >= 1.0f && sensoryGain <= 5.0f);
        
        // Test arousal signaling
        ne.signalArousal(0.6f);
        assert(ne.getLevel() > 0.0f);
        
        ne.signalVigilance(0.7f);
        assert(ne.getLevel() > 0.0f);
        
        // Test plasticity factor
        float plasticity = ne.getPlasticityFactor();
        assert(plasticity >= 0.7f && plasticity <= 1.0f);
        
        std::cout << "✓ Norepinephrine tests passed" << std::endl;
    }
    
    void testSerotonin() {
        std::cout << "Testing Serotonin..." << std::endl;
        
        Serotonin serotonin;
        
        // Test initialization
        assert(serotonin.getName() != nullptr);
        assert(std::string(serotonin.getName()) == "5-HT");
        
        // Test default level
        assert(serotonin.getLevel() == 0.0f);
        
        // Test level setting
        serotonin.setLevel(0.3f);
        assert(serotonin.getLevel() == 0.3f);
        
        // Test learning rate modulation
        serotonin.updateLearningRateModulation();
        float learningRateMod = serotonin.getLearningRateModulation();
        assert(learningRateMod >= 1.0f && learningRateMod <= 3.0f);
        
        // Test impulsivity factor
        float impulsivity = serotonin.getImpulsivityFactor();
        assert(impulsivity > 0.0f && impulsivity <= 1.0f);
        
        // Test mood signaling
        serotonin.signalMood(0.5f);
        assert(serotonin.getLevel() > 0.0f);
        
        serotonin.signalSocialBehavior(0.4f);
        assert(serotonin.getLevel() > 0.0f);
        
        // Test plasticity factor
        float plasticity = serotonin.getPlasticityFactor();
        assert(plasticity >= 0.3f && plasticity <= 1.0f);
        
        std::cout << "✓ Serotonin tests passed" << std::endl;
    }
    
    void testReward() {
        std::cout << "Testing Reward..." << std::endl;
        
        Reward reward;
        
        // Test initialization
        assert(reward.getName() != nullptr);
        assert(std::string(reward.getName()) == "Reward");
        
        // Test default level
        assert(reward.getLevel() == 0.0f);
        
        // Test level setting
        reward.setLevel(0.9f);
        assert(reward.getLevel() == 0.9f);
        
        // Test prediction error signal
        float predError = reward.getPredictionErrorSignal();
        assert(predError == 0.0f);
        
        // Test reward signaling
        reward.signal(0.5f, 0.2f);
        assert(reward.getLevel() > 0.0f);
        
        // Test plasticity factor
        float plasticity = reward.getPlasticityFactor();
        assert(plasticity >= 0.5f && plasticity <= 1.0f);
        
        std::cout << "✓ Reward tests passed" << std::endl;
    }
    
    void runAllNeuromodulationTests() {
        std::cout << "=== Neuromodulation Integration Tests ===" << std::endl;
        
        testAcetylcholine();
        testNorepinephrine();
        testSerotonin();
        testReward();
        
        std::cout << "\n✅ All neuromodulation integration tests passed!" << std::endl;
    }
}

} // namespace nlm

int main() {
    try {
        nlm::runAllNeuromodulationTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}