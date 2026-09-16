// Reward Tests
#include "neuromodulation/Reward.hpp"
#include "world/Observation.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_reward {

void testRewardCreation() {
    nlm::Reward reward;
    
    // Test initial state
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardCreation passed" << std::endl;
}

void testRewardSetValue() {
    nlm::Reward reward;
    
    reward.setValue(0.5f);
    assert(reward.getValue() == 0.5f);
    
    std::cout << "    testRewardSetValue passed" << std::endl;
}

void testRewardAdd() {
    nlm::Reward reward;
    
    reward.add(0.5f);
    assert(reward.getValue() == 0.5f);
    assert(reward.getHistory().size() == 1);
    assert(reward.getHistory()[0] == 0.5f);
    
    reward.add(-0.2f);
    assert(reward.getValue() == 0.3f);
    assert(reward.getHistory().size() == 2);
    assert(reward.getHistory()[1] == -0.2f);
    
    std::cout << "    testRewardAdd passed" << std::endl;
}

void testRewardReset() {
    nlm::Reward reward;
    
    reward.add(0.5f);
    reward.add(0.3f);
    assert(reward.getValue() == 0.8f);
    assert(reward.getHistory().size() == 2);
    
    reward.reset();
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardReset passed" << std::endl;
}

void testRewardComputeReward() {
    nlm::Reward reward;
    
    // Create an observation with some data
    nlm::Observation observation;
    std::vector<float> data;
    
    // Fill with vision data (256 values)
    for (int i = 0; i < 256; ++i) {
        data.push_back(0.5f);  // Medium intensity
    }
    
    // Add some touch data (8 values)
    for (int i = 0; i < 8; ++i) {
        data.push_back(0.8f);  // High touch values
    }
    
    // Add internal signals (4 values)
    for (int i = 0; i < 4; ++i) {
        data.push_back(0.1f);  // Low internal signals
    }
    
    // Add proprioception (6 values)
    for (int i = 0; i < 6; ++i) {
        data.push_back(0.3f);  // Medium proprioception
    }
    
    observation.setData(data);
    
    // Compute reward
    float computedReward = reward.computeReward(observation);
    
    // Check result is within expected range
    assert(computedReward >= -1.0f && computedReward <= 1.0f);
    
    // Test with empty observation
    nlm::Observation emptyObservation;
    float emptyReward = reward.computeReward(emptyObservation);
    assert(emptyReward == 0.0f);
    
    std::cout << "    testRewardComputeReward passed" << std::endl;
}

void testRewardHistoryManagement() {
    nlm::Reward reward;
    
    // Add some rewards
    for (int i = 0; i < 5; ++i) {
        reward.add(static_cast<float>(i) * 0.1f);
    }
    
    assert(reward.getHistory().size() == 5);
    
    // Clear history
    reward.clearHistory();
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardHistoryManagement passed" << std::endl;
}

void runAll() {
    testRewardCreation();
    testRewardSetValue();
    testRewardAdd();
    testRewardReset();
    testRewardComputeReward();
    testRewardHistoryManagement();
    
    std::cout << "\nAll Reward tests passed!" << std::endl;
}

} // namespace test_reward
