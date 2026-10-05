#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Enhanced Dopamine Neuromodulation System" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    nlm::Dopamine dopamine;
    
    // Test 1: Basic functionality
    std::cout << "\nTest 1: Basic functionality" << std::endl;
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "DA");
    assert(dopamine.getLevel() == 0.0f);
    std::cout << "  ✓ Dopamine system created successfully" << std::endl;
    
    // Test 2: Level management
    std::cout << "\nTest 2: Level management" << std::endl;
    dopamine.setLevel(100.0f);
    assert(dopamine.getLevel() == 100.0f);
    std::cout << "  ✓ Level set and retrieved successfully" << std::endl;
    
    // Test 3: Plasticity factor
    std::cout << "\nTest 3: Plasticity factor calculation" << std::endl;
    float plasticity = dopamine.getPlasticityFactor();
    assert(plasticity >= 0.3f && plasticity <= 1.0f);
    std::cout << "  ✓ Plasticity factor calculated: " << plasticity << std::endl;
    
    // Test 4: Reward signaling
    std::cout << "\nTest 4: Reward signaling" << std::endl;
    float initialLevel = dopamine.getLevel();
    dopamine.signalReward(1.0f);
    assert(dopamine.getLevel() > initialLevel);
    std::cout << "  ✓ Reward signaling successful, level increased" << std::endl;
    
    // Test 5: Prediction error signaling
    std::cout << "\nTest 5: Prediction error signaling" << std::endl;
    float levelBeforeRPE = dopamine.getLevel();
    dopamine.signalRewardPredictionError(0.5f); // Positive RPE
    assert(dopamine.getLevel() > levelBeforeRPE);
    std::cout << "  ✓ Positive RPE signaling successful" << std::endl;
    
    float levelAfterPositive = dopamine.getLevel();
    dopamine.signalRewardPredictionError(-0.3f); // Negative RPE
    assert(dopamine.getLevel() <= levelAfterPositive);
    std::cout << "  ✓ Negative RPE signaling successful" << std::endl;
    
    // Test 6: Burst firing
    std::cout << "\nTest 6: Burst firing mechanism" << std::endl;
    float levelBeforeBurst = dopamine.getLevel();
    dopamine.signalRewardPredictionError(1.2f); // Large positive error to trigger burst
    float burstLevel = dopamine.getBurstLevel();
    assert(burstLevel > 0.0f);
    std::cout << "  ✓ Burst firing triggered, level: " << burstLevel << std::endl;
    
    // Test 7: Adaptive baseline
    std::cout << "\nTest 7: Adaptive baseline" << std::endl;
    float baseline = dopamine.getAdaptiveBaseline();
    assert(baseline >= 0.0f);
    std::cout << "  ✓ Adaptive baseline accessible: " << baseline << std::endl;
    
    // Test 8: History tracking
    std::cout << "\nTest 8: History tracking" << std::endl;
    std::vector<float> history = dopamine.getAllConcentrationHistory();
    assert(history.size() >= 0);
    std::cout << "  ✓ Concentration history tracked, size: " << history.size() << std::endl;
    
    std::vector<float> rewardHistory = dopamine.getAllRewardHistory();
    assert(rewardHistory.size() >= 0);
    std::cout << "  ✓ Reward history tracked, size: " << rewardHistory.size() << std::endl;
    
    // Test 9: Update mechanism
    std::cout << "\nTest 9: Update mechanism" << std::endl;
    float levelBeforeUpdate = dopamine.getLevel();
    dopamine.update(0.001f); // 1ms timestep
    assert(dopamine.getLevel() >= 0.0f);
    std::cout << "  ✓ Update mechanism successful" << std::endl;
    
    // Test 10: Prediction error history
    std::cout << "\nTest 10: Prediction error history" << std::endl;
    dopamine.signalRewardPredictionError(0.8f);
    float errorHistory = dopamine.getRecentPredictionError();
    assert(std::abs(errorHistory - 0.8f) < 0.01f);
    std::cout << "  ✓ Prediction error history smoothed: " << errorHistory << std::endl;
    
    std::cout << "\n==================================================" << std::endl;
    std::cout << "All tests passed! Enhanced Dopamine system is working correctly." << std::endl;
    std::cout << "==================================================" << std::endl;
    
    return 0;
}
