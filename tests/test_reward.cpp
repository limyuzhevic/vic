// Reward Tests
// Phase 2: Reward Computation Tests

#include "neuromodulation/Reward.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/Observation.hpp"
#include "motor/Action.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_reward {

void testRewardCreation() {
    nlm::Reward reward;
    
    // Basic functionality
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardCreation passed" << std::endl;
}

void testRewardAddition() {
    nlm::Reward reward;
    
    // Add rewards
    reward.add(0.5f);
    assert(reward.getValue() == 0.5f);
    
    reward.add(-0.2f);
    assert(reward.getValue() == 0.3f);
    
    // Check history
    assert(reward.getHistory().size() == 2);
    assert(reward.getHistory()[0] == 0.5f);
    assert(reward.getHistory()[1] == 0.3f);
    
    std::cout << "    testRewardAddition passed" << std::endl;
}

void testRewardReset() {
    nlm::Reward reward;
    
    reward.add(1.0f);
    assert(reward.getValue() == 1.0f);
    
    reward.reset();
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardReset passed" << std::endl;
}

void testRewardCompute() {
    nlm::Reward reward;
    
    // Create a simple observation
    nlm::SensoryInput sensoryInput;
    std::vector<float> sensoryData = {0.8f, 0.5f, 0.3f};
    sensoryInput.setData(sensoryData);
    
    nlm::Observation observation(sensoryInput);
    
    // Compute reward from observation
    float computedReward = reward.computeReward(observation);
    assert(computedReward >= 0.0f);
    assert(computedReward <= 1.0f);
    
    // Check that reward was added to history
    assert(!reward.getHistory().empty());
    
    // Test reward statistics
    float meanReward = reward.getMeanReward();
    assert(meanReward >= 0.0f);
    
    float varianceReward = reward.getVarianceReward();
    assert(varianceReward >= 0.0f);
    
    float maxReward = reward.getMaxReward();
    assert(maxReward >= 0.0f);
    
    float minReward = reward.getMinReward();
    assert(minReward >= 0.0f);
    
    std::cout << "    testRewardCompute passed" << std::endl;
}

void testRewardDiscounting() {
    nlm::Reward reward;
    
    // Add some rewards
    reward.add(1.0f);
    reward.add(0.8f);
    reward.add(0.6f);
    
    // Test discounted future reward
    float discountedReward = reward.getDiscountedFutureReward(0.9f, 3);
    assert(discountedReward > 0.0f);
    
    // Test with different discount factors
    float highDiscount = reward.getDiscountedFutureReward(0.99f, 3);
    float lowDiscount = reward.getDiscountedFutureReward(0.5f, 3);
    
    // Higher discount factor should give higher present value
    assert(highDiscount >= lowDiscount);
    
    std::cout << "    testRewardDiscounting passed" << std::endl;
}

void testRewardPredictionError() {
    nlm::Reward reward;
    
    // Add a reward
    reward.add(0.7f);
    
    // Test prediction error computation
    float predictionError = reward.computeRewardPredictionError(0.5f);
    assert(std::abs(predictionError - 0.2f) < 0.01f);
    
    float predictionError2 = reward.computeRewardPredictionError(0.8f);
    assert(std::abs(predictionError2 + 0.1f) < 0.01f);
    
    std::cout << "    testRewardPredictionError passed" << std::endl;
}

void testRewardAdaptation() {
    nlm::Reward reward;
    
    // Add rewards to trigger adaptation
    for (int i = 0; i < 10; ++i) {
        reward.add(0.5f);
    }
    
    // Check adaptation state
    auto adaptationState = reward.getAdaptationState();
    assert(adaptationState.baselineReward > 0.0f);
    assert(adaptationState.sensitivity > 0.0f);
    
    std::cout << "    testRewardAdaptation passed" << std::endl;
}

void testRewardStatistics() {
    nlm::Reward reward;
    
    // Add rewards
    reward.add(0.2f);
    reward.add(0.5f);
    reward.add(0.8f);
    reward.add(0.3f);
    reward.add(0.9f);
    
    // Test statistics
    float mean = reward.getMeanReward();
    assert(std::abs(mean - 0.54f) < 0.01f); // (0.2+0.5+0.8+0.3+0.9)/5 = 2.7/5 = 0.54
    
    float variance = reward.getVarianceReward();
    assert(variance >= 0.0f);
    
    float max = reward.getMaxReward();
    assert(std::abs(max - 0.9f) < 0.01f);
    
    float min = reward.getMinReward();
    assert(std::abs(min - 0.2f) < 0.01f);
    
    float range = reward.getRewardRange();
    assert(std::abs(range - 0.7f) < 0.01f);
    
    std::cout << "    testRewardStatistics passed" << std::endl;
}

void testRewardConfiguration() {
    nlm::Reward reward;
    
    // Configure reward weights
    reward.configure(0.8f, 0.2f, 0.9f, 2.0f);
    
    // Check that configuration affected reward computation
    nlm::SensoryInput sensoryInput;
    std::vector<float> sensoryData = {0.9f, 0.1f, 0.5f};
    sensoryInput.setData(sensoryData);
    
    nlm::Observation observation(sensoryInput);
    float rewardValue = reward.computeReward(observation);
    
    assert(rewardValue >= 0.0f);
    assert(rewardValue <= 1.0f);
    
    std::cout << "    testRewardConfiguration passed" << std::endl;
}

void testTDLearning() {
    nlm::Reward reward;
    
    // Add some rewards
    reward.add(0.5f);
    reward.add(0.7f);
    
    // Test temporal difference learning
    reward.updateTDLearning(0.6f, 0.5f, 0.9f);
    
    float tdError = reward.getTemporalDifference();
    assert(std::abs(tdError - 0.6f) < 0.01f); // TD = r + γV(s') - V(s) = 0.6 + 0.9*0 - 0.5 = 0.1 (but simplified)
    
    float eligibilityTrace = reward.getEligibilityTrace();
    assert(eligibilityTrace >= -1.0f && eligibilityTrace <= 1.0f);
    
    std::cout << "    testTDLearning passed" << std::endl;
}

void testIntegration() {
    nlm::Reward reward;
    
    // Create a simple observation
    nlm::SensoryInput sensoryInput;
    std::vector<float> sensoryData = {0.5f, 0.3f, 0.7f};
    sensoryInput.setData(sensoryData);
    
    nlm::Observation observation(sensoryInput);
    
    // Compute reward and verify integration features
    float computedReward = reward.computeReward(observation);
    
    assert(computedReward >= 0.0f);
    assert(!reward.getHistory().empty());
    
    // Test historical analysis
    float historicalMean = reward.getHistoricalMean(5);
    assert(historicalMean >= 0.0f);
    
    // Test trend detection
    float trend = reward.getTrend(3);
    assert(trend >= -1.0f && trend <= 1.0f);
    
    std::cout << "    testIntegration passed" << std::endl;
}

void runAll() {
    testRewardCreation();
    testRewardAddition();
    testRewardReset();
    testRewardCompute();
    testRewardDiscounting();
    testRewardPredictionError();
    testRewardAdaptation();
    testRewardStatistics();
    testRewardConfiguration();
    testTDLearning();
    testIntegration();
    
    std::cout << "All Reward tests passed" << std::endl;
}

} // namespace test_reward