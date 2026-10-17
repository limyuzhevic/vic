// Reward Tests
#include "neuromodulation/Reward.hpp"
#include "environment/Environment.hpp"
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

void testRewardValue() {
    nlm::Reward reward;
    
    reward.setValue(0.5f);
    assert(reward.getValue() == 0.5f);
    
    reward.add(0.3f);
    assert(reward.getValue() == 0.8f);
    
    std::cout << "    testRewardValue passed" << std::endl;
}

void testRewardReset() {
    nlm::Reward reward;
    
    reward.add(1.0f);
    reward.add(-0.5f);
    assert(reward.getValue() == 0.5f);
    
    reward.reset();
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardReset passed" << std::endl;
}

void testRewardHistory() {
    nlm::Reward reward;
    
    assert(reward.getHistory().empty());
    
    reward.add(0.1f);
    reward.add(0.2f);
    reward.add(-0.1f);
    
    assert(reward.getHistory().size() == 3);
    assert(reward.getHistory()[0] == 0.1f);
    assert(reward.getHistory()[1] == 0.2f);
    assert(reward.getHistory()[2] == -0.1f);
    
    reward.clearHistory();
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardHistory passed" << std::endl;
}

void testRewardVectorObservation() {
    nlm::Reward reward;
    
    // Create a simple observation with vector-based position
    struct VectorObservation {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        
        VectorObservation() : x(0.0f), y(0.0f), z(0.0f) {}
        VectorObservation(float x, float y, float z) : x(x), y(y), z(z) {}
    };
    
    // Test goal-seeking behavior
    VectorObservation goal1(0.8f, 0.8f, 0.0f);  // Goal at (0.8, 0.8)
    
    // Agent at start position (0.0, 0.0)
    VectorObservation agent1(0.0f, 0.0f, 0.0f);
    float reward1 = reward.computeReward(agent1);
    assert(reward1 < 0.0f);  // Should be negative (far from goal)
    
    // Agent closer to goal
    VectorObservation agent2(0.5f, 0.5f, 0.0f);
    float reward2 = reward.computeReward(agent2);
    assert(reward2 > reward1);  // Should be better (closer to goal)
    
    // Agent at goal
    VectorObservation agent3(0.8f, 0.8f, 0.0f);
    float reward3 = reward.computeReward(agent3);
    assert(reward3 > reward2);  // Should be even better (at goal)
    
    // Test boundary conditions
    VectorObservation agent4(-1.0f, -1.0f, 0.0f);  // Outside grid
    float reward4 = reward.computeReward(agent4);
    assert(reward4 < 0.0f);  // Should be negative
    
    std::cout << "    testRewardVectorObservation passed" << std::endl;
}

void testRewardHistoryAccumulation() {
    nlm::Reward reward;
    
    // Add multiple rewards
    reward.add(0.5f);
    reward.add(0.3f);
    reward.add(-0.2f);
    
    // Check accumulated value
    assert(reward.getValue() == 0.6f);
    
    // Check history
    assert(reward.getHistory().size() == 3);
    assert(reward.getHistory()[0] == 0.5f);
    assert(reward.getHistory()[1] == 0.3f);
    assert(reward.getHistory()[2] == -0.2f);
    
    // Reset should clear both value and history
    reward.reset();
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardHistoryAccumulation passed" << std::endl;
}

void testRewardBounds() {
    nlm::Reward reward;
    
    // Test reward clamping
    reward.setValue(1.5f);
    assert(reward.getValue() <= 5.0f);  // Should be clamped
    
    reward.setValue(-1.5f);
    assert(reward.getValue() >= -5.0f);  // Should be clamped
    
    std::cout << "    testRewardBounds passed" << std::endl;
}

void testRewardComputeConsistent() {
    nlm::Reward reward;
    
    // Same observation should give same reward
    struct VectorObservation {
        float x = 0.0f;
        float y = 0.0f;
    } obs1(0.3f, 0.7f), obs2(0.3f, 0.7f);
    
    float reward1 = reward.computeReward(obs1);
    float reward2 = reward.computeReward(obs2);
    
    assert(std::abs(reward1 - reward2) < 0.001f);
    
    std::cout << "    testRewardComputeConsistent passed" << std::endl;
}

void testRewardDefaultBehavior() {
    nlm::Reward reward;
    
    // Default observation (uninitialized) should return small exploration bonus
    struct VectorObservation {
        float x = 0.0f;
        float y = 0.0f;
    } obs;
    
    float reward = reward.computeReward(obs);
    // Should be positive (exploration bonus)
    assert(reward > 0.0f);
    
    std::cout << "    testRewardDefaultBehavior passed" << std::endl;
}

void runAll() {
    std::cout << "Running Reward tests..." << std::endl;
    testRewardCreation();
    testRewardValue();
    testRewardReset();
    testRewardHistory();
    testRewardVectorObservation();
    testRewardHistoryAccumulation();
    testRewardBounds();
    testRewardComputeConsistent();
    testRewardDefaultBehavior();
}

} // namespace test_reward