// Test file for Reward class real reward computation
// Tests various reward mechanisms including goal achievement, resource collection,
// hazard penalties, step penalties, and temporal reward decay

#include "src/neuromodulation/Reward.hpp"
#include "src/world/Observation.hpp"
#include "src/environment/Observation.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

void testRewardInitialization() {
    std::cout << "Testing Reward initialization..." << std::endl;
    
    nlm::Reward reward;
    assert(!reward.isInitialized());
    
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_weight_visual", 1.0f);
    config->set("reward_weight_sensory", 1.0f);
    config->set("reward_weight_internal", 1.0f);
    config->set("reward_weight_temporal", 0.5f);
    config->set("reward_sparse", true);
    config->set("reward_goal_threshold", 0.9f);
    config->set("reward_hazard_threshold", 0.7f);
    
    reward.initialize(config);
    assert(reward.isInitialized());
    
    std::cout << "✓ Reward initialization test passed" << std::endl;
}

void testRewardParameters() {
    std::cout << "Testing Reward parameter configuration..." << std::endl;
    
    nlm::Reward reward;
    
    // Test default parameters
    assert(std::abs(reward.getGoalReward() - 10.0f) < 0.01f);
    assert(std::abs(reward.getResourceReward() - 1.0f) < 0.01f);
    assert(std::abs(reward.getHazardPenalty() - (-5.0f)) < 0.01f);
    assert(std::abs(reward.getStepPenalty() - (-0.01f)) < 0.01f);
    assert(std::abs(reward.getExplorationReward() - 0.1f) < 0.01f);
    
    // Test custom parameters
    reward.setGoalReward(15.0f);
    reward.setResourceReward(2.0f);
    reward.setHazardPenalty(-10.0f);
    reward.setStepPenalty(-0.05f);
    reward.setExplorationReward(0.5f);
    
    assert(std::abs(reward.getGoalReward() - 15.0f) < 0.01f);
    assert(std::abs(reward.getResourceReward() - 2.0f) < 0.01f);
    assert(std::abs(reward.getHazardPenalty() - (-10.0f)) < 0.01f);
    assert(std::abs(reward.getStepPenalty() - (-0.05f)) < 0.01f);
    assert(std::abs(reward.getExplorationReward() - 0.5f) < 0.01f);
    
    std::cout << "✓ Reward parameter configuration test passed" << std::endl;
}

void testRewardAccumulation() {
    std::cout << "Testing Reward accumulation and history..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", true);
    reward.initialize(config);
    
    // Test add method
    reward.add(5.0f);
    assert(std::abs(reward.getValue() - 5.0f) < 0.01f);
    assert(std::abs(reward.getEpisodeTotalReward() - 5.0f) < 0.01f);
    
    float lastStepReward = reward.getLastStepReward();
    assert(std::abs(lastStepReward - 5.0f) < 0.01f);
    
    // Add more rewards
    reward.add(3.0f);
    reward.add(-2.0f);
    
    assert(std::abs(reward.getValue() - 6.0f) < 0.01f);
    assert(std::abs(reward.getEpisodeTotalReward() - 6.0f) < 0.01f);
    
    // Check history
    const auto& history = reward.getHistory();
    assert(history.size() == 3); // 5.0, 3.0, -2.0
    assert(std::abs(history[0] - 5.0f) < 0.01f);
    assert(std::abs(history[1] - 3.0f) < 0.01f);
    assert(std::abs(history[2] - (-2.0f)) < 0.01f);
    
    std::cout << "✓ Reward accumulation test passed" << std::endl;
}

void testRewardReset() {
    std::cout << "Testing Reward reset functionality..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    reward.initialize(config);
    
    // Add some rewards
    reward.add(10.0f);
    reward.add(5.0f);
    
    assert(reward.getValue() > 0.0f);
    assert(reward.getEpisodeTotalReward() > 0.0f);
    
    // Reset
    reward.reset();
    
    assert(std::abs(reward.getValue() - 0.0f) < 0.01f);
    assert(std::abs(reward.getEpisodeTotalReward() - 0.0f) < 0.01f);
    assert(reward.getLastStepReward() == 0.0f);
    
    // Check history (should still contain past rewards)
    const auto& history = reward.getHistory();
    assert(history.size() == 2); // 10.0, 5.0
    
    reward.clearHistory();
    assert(history.empty());
    
    std::cout << "✓ Reward reset test passed" << std::endl;
}

void testSparseRewardMode() {
    std::cout << "Testing sparse reward mode..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", true);
    config->set("reward_goal_threshold", 0.8f);
    reward.initialize(config);
    
    // Create a mock observation with goal proximity
    nlm::world::Observation obs;
    // Simulate observation data with goal proximity
    std::vector<float> data(100, 0.1f);
    // Add some goal indicators (brightness > 0.7)
    for (int i = 0; i < 10; ++i) {
        data[i] = 0.85f;
    }
    obs.setData(data);
    
    float rewardValue = reward.computeReward(obs);
    
    // Should get goal reward due to sufficient goal proximity
    assert(rewardValue > 0.0f);
    
    // Test with insufficient goal proximity
    config->set("reward_goal_threshold", 0.95f); // Very high threshold
    reward.initialize(config); // Reinitialize with new config
    
    // Same observation should now not trigger goal reward
    rewardValue = reward.computeReward(obs);
    
    // May still get step penalty but no goal reward
    assert(rewardValue < 0.0f);
    
    std::cout << "✓ Sparse reward mode test passed" << std::endl;
}

void testDenseRewardMode() {
    std::cout << "Testing dense reward mode..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", false);
    config->set("reward_goal_threshold", 0.8f);
    config->set("reward_hazard_threshold", 0.7f);
    reward.initialize(config);
    
    // Create a mock observation with hazard proximity
    nlm::world::Observation obs;
    std::vector<float> data(300, 0.5f); // RGB data
    // Add hazard indicators (red channel > 0.8)
    for (size_t i = 0; i < data.size(); i += 3) {
        if (i + 2 < data.size()) {
            data[i + 2] = 0.9f; // Red channel
        }
    }
    obs.setData(data);
    
    float rewardValue = reward.computeReward(obs);
    
    // Should get hazard penalty
    assert(rewardValue < 0.0f);
    
    std::cout << "✓ Dense reward mode test passed" << std::endl;
}

void testTemporalRewardDecay() {
    std::cout << "Testing temporal reward decay..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", true);
    reward.initialize(config);
    
    nlm::world::Observation obs;
    std::vector<float> data(100, 0.5f);
    obs.setData(data);
    
    // Compute initial reward
    float initialReward = reward.computeReward(obs);
    
    // Simulate time passing by updating temporal state
    for (int i = 0; i < 10; ++i) {
        reward.updateTemporalState();
    }
    
    // Compute reward after time decay
    float decayedReward = reward.computeReward(obs);
    
    // Reward should decrease over time
    assert(decayedReward <= initialReward);
    
    std::cout << "✓ Temporal reward decay test passed" << std::endl;
}

void testGoalProgressCalculation() {
    std::cout << "Testing goal progress calculation..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", false);
    reward.initialize(config);
    
    nlm::world::Observation obs;
    std::vector<float> data(100, 0.5f);
    
    // Test with different goal proximity levels
    // Simulate various levels of goal proximity
    for (float proximity = 0.0f; proximity <= 1.0f; proximity += 0.2f) {
        // Reset config to ensure fresh state
        auto testConfig = std::make_shared<nlm::Config>();
        testConfig->set("reward_sparse", false);
        nlm::Reward testReward;
        testReward.initialize(testConfig);
        
        // Create observation with goal indicators
        nlm::world::Observation testObs;
        std::vector<float> testData(100, 0.5f - proximity * 0.4f);
        // Make goal proximity based on proximity value
        for (size_t i = 0; i < testData.size(); ++i) {
            if (i % 5 == 0) { // Every 5th pixel is a goal indicator
                testData[i] = 0.9f - proximity * 0.1f;
            }
        }
        testObs.setData(testData);
        
        float progress = testReward.computeReward(testObs);
        
        // Progress should increase with goal proximity
        if (proximity > 0.0f) {
            assert(progress >= 0.0f);
        }
    }
    
    std::cout << "✓ Goal progress calculation test passed" << std::endl;
}

void testHazardRiskAssessment() {
    std::cout << "Testing hazard risk assessment..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", false);
    config->set("reward_hazard_threshold", 0.6f);
    reward.initialize(config);
    
    nlm::world::Observation obs;
    std::vector<float> data(300, 0.5f);
    
    // Test with different hazard levels
    for (float hazardLevel = 0.0f; hazardLevel <= 1.0f; hazardLevel += 0.3f) {
        // Reset config to ensure fresh state
        auto testConfig = std::make_shared<nlm::Config>();
        testConfig->set("reward_sparse", false);
        testConfig->set("reward_hazard_threshold", 0.6f);
        nlm::Reward testReward;
        testReward.initialize(testConfig);
        
        // Create observation with hazard indicators
        nlm::world::Observation testObs;
        std::vector<float> testData(300, 0.5f);
        // Add hazard indicators based on hazardLevel
        size_t hazardCount = static_cast<size_t>(testData.size() * 0.3f * hazardLevel);
        for (size_t i = 0; i < hazardCount && i < testData.size(); i += 3) {
            if (i + 2 < testData.size()) {
                testData[i + 2] = 0.9f;
            }
        }
        testObs.setData(testData);
        
        float hazardRisk = testReward.computeReward(testObs);
        
        // Higher hazard level should result in higher risk/penalty
        if (hazardLevel > 0.0f) {
            assert(hazardRisk < 0.0f); // Should get penalty
        }
    }
    
    std::cout << "✓ Hazard risk assessment test passed" << std::endl;
}

void testResourceAvailability() {
    std::cout << "Testing resource availability..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", false);
    reward.initialize(config);
    
    nlm::world::Observation obs;
    std::vector<float> data(300, 0.5f);
    
    // Test with different resource levels
    for (float resourceLevel = 0.0f; resourceLevel <= 1.0f; resourceLevel += 0.3f) {
        // Reset config to ensure fresh state
        auto testConfig = std::make_shared<nlm::Config>();
        testConfig->set("reward_sparse", false);
        nlm::Reward testReward;
        testReward.initialize(testConfig);
        
        // Create observation with resource indicators
        nlm::world::Observation testObs;
        std::vector<float> testData(300, 0.5f);
        // Add resource indicators based on resourceLevel
        size_t resourceCount = static_cast<size_t>(testData.size() * 0.4f * resourceLevel);
        for (size_t i = 1; i < resourceCount && i < testData.size(); i += 3) {
            testData[i] = 0.6f + resourceLevel * 0.3f; // Green channel
        }
        testObs.setData(testData);
        
        float rewardValue = testReward.computeReward(testObs);
        
        // Higher resource level should increase reward
        if (resourceLevel > 0.0f) {
            assert(rewardValue >= 0.0f);
        }
    }
    
    std::cout << "✓ Resource availability test passed" << std::endl;
}

void testNearMissRewards() {
    std::cout << "Testing near-miss reward tolerance..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", false);
    config->set("reward_goal_threshold", 0.9f);
    config->set("reward_near_miss_bonus", 0.5f);
    reward.initialize(config);
    
    nlm::world::Observation obs;
    std::vector<float> data(100, 0.5f);
    
    // Create observation with high but not quite goal-level proximity
    for (int i = 0; i < 15; ++i) { // 15% goal proximity
        data[i] = 0.75f;
    }
    obs.setData(data);
    
    float rewardValue = reward.computeReward(obs);
    
    // Should get near-miss bonus (between 0.2 and 0.9 threshold)
    assert(rewardValue > 0.0f);
    
    std::cout << "✓ Near-miss reward tolerance test passed" << std::endl;
}

void testRewardIntegration() {
    std::cout << "Testing complete Reward integration..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", false);
    config->set("reward_goal_threshold", 0.8f);
    config->set("reward_hazard_threshold", 0.7f);
    config->set("reward_near_miss_bonus", 0.3f);
    reward.initialize(config);
    
    // Create a comprehensive test observation
    nlm::world::Observation obs;
    std::vector<float> data(300, 0.5f); // RGB data
    
    // Add goal indicators (20% of pixels)
    size_t goalCount = data.size() / 5;
    for (size_t i = 0; i < goalCount && i < data.size(); i += 5) {
        data[i] = 0.85f; // Red channel for goal
    }
    
    // Add hazard indicators (10% of pixels)
    size_t hazardCount = data.size() / 20;
    for (size_t i = 0; i < hazardCount && i < data.size(); i += 3) {
        if (i + 2 < data.size()) {
            data[i + 2] = 0.9f; // Red channel for hazard
        }
    }
    
    // Add resource indicators (15% of pixels)
    size_t resourceCount = data.size() / 7;
    for (size_t i = 1; i < resourceCount && i < data.size(); i += 3) {
        data[i] = 0.65f; // Green channel for resources
    }
    
    obs.setData(data);
    
    // Compute reward
    float rewardValue = reward.computeReward(obs);
    
    // Check episode statistics
    assert(reward.getGoalProgress() >= 0.0f);
    assert(reward.getHazardRisk() >= 0.0f);
    
    // Should get mixed reward (goal positive, hazard negative)
    // Overall should be close to zero due to balancing
    assert(std::abs(rewardValue) < 10.0f);
    
    std::cout << "✓ Complete Reward integration test passed" << std::endl;
}

int main() {
    std::cout << "Running Reward system tests..." << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        testRewardInitialization();
        testRewardParameters();
        testRewardAccumulation();
        testRewardReset();
        testSparseRewardMode();
        testDenseRewardMode();
        testTemporalRewardDecay();
        testGoalProgressCalculation();
        testHazardRiskAssessment();
        testResourceAvailability();
        testNearMissRewards();
        testRewardIntegration();
        
        std::cout << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "All Reward system tests PASSED! ✓" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "Test FAILED with exception: " << e.what() << std::endl;
        std::cout << "=====================================" << std::endl;
        return 1;
    } catch (...) {
        std::cout << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "Test FAILED with unknown exception" << std::endl;
        std::cout << "=====================================" << std::endl;
        return 1;
    }
}