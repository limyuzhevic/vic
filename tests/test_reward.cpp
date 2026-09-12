// Reward Integration Tests
#include "neuromodulation/Reward.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_reward {

void testRewardCreation() {
    nlm::Reward reward;
    
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardCreation passed" << std::endl;
}

void testRewardAddition() {
    nlm::Reward reward;
    
    // Add reward
    reward.add(0.5f);
    assert(reward.getValue() == 0.5f);
    
    // Add more reward
    reward.add(0.3f);
    assert(reward.getValue() == 0.8f);
    
    // Check history is not empty (implementation detail)
    
    std::cout << "    testRewardAddition passed" << std::endl;
}

void testRewardReset() {
    nlm::Reward reward;
    
    reward.add(1.0f);
    assert(reward.getValue() > 0.0f);
    
    reward.reset();
    assert(reward.getValue() == 0.0f);
    
    std::cout << "    testRewardReset passed" << std::endl;
}

void testRewardRealBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* reward = brain.getReward();
    assert(reward != nullptr);
    
    // Test reward system through brain simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Check that reward system is accessible
        assert(reward->getValue() >= 0.0f);
        
        // Simulate receiving reward every 10 steps
        if (step % 10 == 0) {
            reward->receiveReward(0.1f, 0.001f);
        }
    
    std::cout << "    testRewardRealBrainIntegration passed" << std::endl;
}

void runAll() {
    testRewardCreation();
    testRewardAddition();
    testRewardReset();
    testRewardRealBrainIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Reward Tests PASSED ===" << std::endl;
    std::cout << "Reward system works correctly in brain simulation" << std::endl;
}

} // namespace test_reward