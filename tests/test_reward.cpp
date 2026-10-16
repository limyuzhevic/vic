// Reward Tests
#include "neuromodulation/Reward.hpp"
#include "world/Observation.hpp"
#include "environment/Observation.hpp"
#include "sensory/SensoryInput.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_reward {

void testRewardCreation() {
    nlm::Reward reward;
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    std::cout << "    testRewardCreation passed" << std::endl;
}

void testRewardValueManagement() {
    nlm::Reward reward;
    reward.setValue(5.0f);
    assert(reward.getValue() == 5.0f);
    
    reward.add(2.0f);
    assert(reward.getValue() == 7.0f);
    
    reward.reset();
    assert(reward.getValue() == 0.0f);
    std::cout << "    testRewardValueManagement passed" << std::endl;
}

void testRewardHistory() {
    nlm::Reward reward;
    
    // Add rewards
    reward.add(1.0f);
    reward.add(-0.5f);
    reward.add(2.0f);
    
    assert(reward.getHistory().size() == 3);
    assert(reward.getHistory()[0] == 1.0f);
    assert(reward.getHistory()[1] == -0.5f);
    assert(reward.getHistory()[2] == 2.0f);
    assert(reward.getHistoryTimestamps().size() == 3);
    
    reward.clearHistory();
    assert(reward.getHistory().empty());
    assert(reward.getHistoryTimestamps().empty());
    std::cout << "    testRewardHistory passed" << std::endl;
}

void testRewardNormalization() {
    nlm::Reward reward;
    
    // Test normalization with different values
    float norm1 = reward.normalizeReward(0.5f, -1.0f, 1.0f);
    assert(std::abs(norm1 - 0.25f) < 0.01f);  // (0.5 - (-1)) / (1 - (-1)) = 0.75
    
    float norm2 = reward.normalizeReward(-0.8f, -1.0f, 1.0f);
    assert(std::abs(norm2 - (-0.4f)) < 0.01f);  // (-0.8 - (-1)) / (1 - (-1)) = 0.1 / 2 = 0.05, then tanh(0.05)*0.5 = ~0.025
    
    float norm3 = reward.normalizeReward(0.0f, -1.0f, 1.0f);
    assert(std::abs(norm3 - 0.0f) < 0.01f);  // tanh(0)*0.5 = 0
    
    std::cout << "    testRewardNormalization passed" << std::endl;
}

void testRewardSignificance() {
    nlm::Reward reward;
    
    assert(reward.isSignificant(0.1f, 0.01f));  // Above threshold
    assert(reward.isSignificant(-0.05f, 0.01f));  // Below threshold
    assert(!reward.isSignificant(0.001f, 0.01f));  // Below threshold
    
    std::cout << "    testRewardSignificance passed" << std::endl;
}

void testRewardDecay() {
    nlm::Reward reward;
    reward.setValue(10.0f);
    
    float beforeDecay = reward.getValue();
    reward.applyDecay(0.1f);  // 10% decay
    float afterDecay = reward.getValue();
    
    assert(std::abs(afterDecay - (beforeDecay * 0.9f)) < 0.001f);
    std::cout << "    testRewardDecay passed" << std::endl;
}

void testRewardTypes() {
    nlm::Reward reward;
    
    // Test different reward types
    reward.setRewardType(nlm::RewardType::Extrinsic, 1.0f);
    assert(reward.getDominantRewardType() == nlm::RewardType::Extrinsic);
    
    reward.setRewardType(nlm::RewardType::Intrinsic, 0.5f);
    assert(reward.getDominantRewardType() == nlm::RewardType::Intrinsic);
    
    reward.setRewardType(nlm::RewardType::Mixed, 1.0f);
    assert(reward.getDominantRewardType() == nlm::RewardType::Mixed);
    
    std::cout << "    testRewardTypes passed" << std::endl;
}

void testRewardComponents() {
    nlm::Reward reward;
    
    // Set up reward components
    reward.setRewardType(nlm::RewardType::Extrinsic, 1.0f);
    reward.setRewardType(nlm::RewardType::Intrinsic, 0.5f);
    
    // Add some rewards to generate components
    reward.add(1.0f);
    reward.add(-0.5f);
    
    const auto& components = reward.getComponents();
    assert(components.size() >= 2);  // At least extrinsic and intrinsic components
    
    float totalWeighted = reward.getTotalWeightedReward();
    assert(totalWeighted >= 0.0f);  // Should be positive or zero
    
    std::cout << "    testRewardComponents passed" << std::endl;
}

void testRewardPredictionError() {
    nlm::Reward reward;
    
    float error = reward.computePredictionError(0.5f, 0.8f);
    assert(std::abs(error - 0.3f) < 0.001f);  // 0.8 - 0.5 = 0.3
    
    float error2 = reward.computePredictionError(1.0f, 0.2f);
    assert(std::abs(error2 - (-0.8f)) < 0.001f);  // 0.2 - 1.0 = -0.8
    
    std::cout << "    testRewardPredictionError passed" << std::endl;
}

void testExplorationBonus() {
    nlm::Reward reward;
    
    // Test with high uncertainty
    float bonus1 = reward.computeExplorationBonus(0.8f, 0.2f);
    assert(bonus1 > 0.0f);  // Should encourage exploration
    
    // Test with low uncertainty
    float bonus2 = reward.computeExplorationBonus(0.1f, 0.2f);
    assert(bonus2 >= 0.0f);  // Should be non-negative
    
    std::cout << "    testExplorationBonus passed" << std::endl;
}

// Mock Observation for testing
class MockObservation {
public:
    MockObservation() {}
    
    void setData(const std::vector<float>& data) {
        data_.assign(data.begin(), data.end());
    }
    
    const std::vector<float>& getData() const {
        return data_;
    }
    
    size_t getDimensions() const {
        return data_.size();
    }
    
    void setFromPercept(const nlm::SensoryPercept& percept) {
        // Mock implementation
    }
    
private:
    std::vector<float> data_;
};

void testComputeRewardWithObservation() {
    nlm::Reward reward;
    
    MockObservation observation;
    
    // Test with positive observation
    observation.setData({1.0f, 2.0f, 3.0f});  // Average = 2.0
    float reward1 = reward.computeReward(observation, nlm::RewardType::Extrinsic);
    assert(reward1 > 0.0f);  // Should be positive
    
    // Test with negative observation
    observation.setData({-1.0f, -2.0f, -3.0f});  // Average = -2.0
    float reward2 = reward.computeReward(observation, nlm::RewardType::Extrinsic);
    assert(reward2 < 0.0f);  // Should be negative
    
    // Test with zero observation
    observation.setData({0.0f, 0.0f, 0.0f});  // Average = 0.0
    float reward3 = reward.computeReward(observation, nlm::RewardType::Extrinsic);
    assert(std::abs(reward3) < 0.1f);  // Should be near zero
    
    std::cout << "    testComputeRewardWithObservation passed" << std::endl;
}

void runAll() {
    testRewardCreation();
    testRewardValueManagement();
    testRewardHistory();
    testRewardNormalization();
    testRewardSignificance();
    testRewardDecay();
    testRewardTypes();
    testRewardComponents();
    testRewardPredictionError();
    testExplorationBonus();
    testComputeRewardWithObservation();
}

} // namespace test_reward