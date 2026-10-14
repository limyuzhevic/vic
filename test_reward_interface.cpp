// Simple interface test for Reward class
// This tests that all required methods exist and compile

#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Reward.hpp"

int main() {
    // Test that Reward is properly instantiable
    nlm::Reward reward;
    
    // Test Neuromodulator interface methods
    const char* name = reward.getName();
    float level = reward.getLevel();
    reward.setLevel(0.5f);
    float plasticity = reward.getPlasticityFactor();
    reward.update(0.001);
    
    // Test Reward-specific methods
    nlm::Observation obs;
    float computedReward = reward.computeReward(obs);
    reward.computePredictionError(0.0f);
    float predError = reward.getRewardPredictionError();
    float value = reward.getValue();
    reward.setValue(0.8f);
    reward.add(0.1f);
    reward.reset();
    const std::vector<float>& history = reward.getHistory();
    reward.clearHistory();
    
    // Test integration methods (these will compile but won't have implementations yet)
    class MockBrain {};
    class MockAttentionalSelection {};
    class MockDopamine {};
    
    reward.integrateWithMemory(nullptr);
    reward.applyToAttention(nullptr);
    reward.signalToDopamine(nullptr);
    
    return 0;
}
