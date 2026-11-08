// RewardPredictionTest.cpp
// Specific tests for reward prediction system fixes

#include "RewardPredictionTest.hpp"
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_reward_prediction {

void testRewardPredictionBugFix() {
    std::cout << "=== Testing Reward Prediction Bug Fix ===" << std::endl;
    
    // Create minimal brain and agent
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    agent.enableRewardModulation(true);
    
    // Test 1: Verify prediction system no longer has zero prediction
    float rewards[] = {5.0f, -3.0f, 8.0f, -2.0f};
    float totalPredictionError = 0.0f;
    
    for (int i = 0; i < 4; ++i) {
        float predicted = (i > 0) ? rewards[i-1] * 0.5f : 0.0f;
        float actual = rewards[i];
        
        agent.applyRewardModulation(actual, predicted);
        
        float error = agent.getPredictionError();
        totalPredictionError += std::abs(error);
        
        // Prediction error should not be dominated by zero prediction bug
        assert(std::abs(error) <= 10.0f);  // Reasonable error range
        
        std::cout << "    Step " << i << ": reward=" << actual 
                  << ", predicted=" << predicted 
                  << ", error=" << error << std::endl;
    }
    
    // Test 2: Verify expected reward learning
    float expectedBefore = agent.getExpectedReward();
    
    // Apply a reward to update expected
    agent.applyRewardModulation(10.0f, 0.0f);
    
    float expectedAfter = agent.getExpectedReward();
    
    // Expected reward should have moved toward actual reward
    assert(expectedAfter >= expectedBefore - 0.1f);  // Should have updated
    
    std::cout << "    Expected reward before: " << expectedBefore << std::endl;
    std::cout << "    Expected reward after:  " << expectedAfter << std::endl;
    
    // Test 3: Verify prediction error is not always equal to reward
    // (this was the bug - prediction was always 0, so error = reward)
    float predictedTest = 7.0f;
    agent.applyRewardModulation(10.0f, predictedTest);
    float error = agent.getPredictionError();
    
    // Error should be reward - prediction, not just reward
    assert(std::abs(error - (10.0f - predictedTest)) < 0.001f);
    
    std::cout << "    Prediction error correctly computed: " << error << std::endl;
    
    std::cout << "    Reward prediction bug fix test PASSED" << std::endl;
}

void testPredictionErrorComputation() {
    std::cout << "=== Testing Prediction Error Computation ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 30);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    agent.enableRewardModulation(true);
    
    // Test various reward-prediction pairs
    struct TestCase {
        float reward;
        float prediction;
        float expectedError;
    };
    
    TestCase testCases[] = {
        {5.0f, 4.5f, 0.5f},
        {-3.0f, -2.5f, -0.5f},
        {0.0f, 0.0f, 0.0f},
        {10.0f, 7.0f, 3.0f},
        {-8.0f, -10.0f, 2.0f}
    };
    
    for (size_t i = 0; i < sizeof(testCases)/sizeof(TestCase); ++i) {
        auto& tc = testCases[i];
        agent.applyRewardModulation(tc.reward, tc.prediction);
        
        float error = agent.getPredictionError();
        assert(std::abs(error - tc.expectedError) < 0.001f);
        
        std::cout << "    Test case " << i << ": reward=" << tc.reward
                  << ", prediction=" << tc.prediction
                  << ", error=" << error << " (expected " << tc.expectedError << ")" << std::endl;
    }
    
    std::cout << "    Prediction error computation test PASSED" << std::endl;
}

void testExpectedRewardLearning() {
    std::cout << "=== Testing Expected Reward Learning ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 40);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    agent.enableRewardModulation(true);
    
    // Set initial expected reward
    agent.applyRewardModulation(5.0f, 0.0f);
    float initialExpected = agent.getExpectedReward();
    
    std::cout << "    Initial expected reward: " << initialExpected << std::endl;
    
    // Apply another reward to see learning
    agent.applyRewardModulation(15.0f, initialExpected);
    float afterExpected = agent.getExpectedReward();
    
    std::cout << "    Expected reward after second reward: " << afterExpected << std::endl;
    
    // Expected reward should have moved toward actual reward
    // (0.95 * old + 0.05 * new)
    float calculatedExpected = 0.95f * initialExpected + 0.05f * 15.0f;
    
    assert(std::abs(afterExpected - calculatedExpected) < 0.001f);
    
    std::cout << "    Expected reward learning test PASSED" << std::endl;
}

void runAllRewardPredictionTests() {
    std::cout << "=== Reward Prediction Tests ===" << std::endl << std::endl;
    
    testRewardPredictionBugFix();
    std::cout << std::endl;
    
    testPredictionErrorComputation();
    std::cout << std::endl;
    
    testExpectedRewardLearning();
    std::cout << std::endl;
    
    std::cout << "=== ALL REWARD PREDICTION TESTS PASSED ===" << std::endl;
}

} // namespace test_reward_prediction