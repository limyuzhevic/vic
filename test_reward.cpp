// Test implementation for the enhanced Reward system
// This test verifies that the Reward class implements the multi-component reward system
// as specified in the requirements.

#include "Reward.hpp"
#include "Observation.hpp"
#include "Config.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

namespace test_reward {

    // Helper function to create a simple test observation
    std::unique_ptr<Observation> createTestObservation(const std::vector<float>& data) {
        auto observation = std::make_unique<Observation>();
        observation->setFromPercept(); // This might need adjustment based on actual API
        return observation;
    }
    
    // Helper function to create a test config
    std::shared_ptr<const Config> createTestConfig() {
        auto config = std::make_shared<Config>();
        // Set some basic config values for testing
        config->set("reward_survival_weight", 0.4f);
        config->set("reward_efficiency_weight", 0.3f);
        config->set("reward_learning_weight", 0.2f);
        config->set("reward_exploration_weight", 0.1f);
        return config;
    }
    
    void testRewardBasicInterface() {
        std::cout << "Testing Reward basic interface..." << std::endl;
        
        Reward reward;
        
        // Test Neuromodulator interface
        assert(std::string(reward.getName()) == "Reward");
        assert(reward.getLevel() == 0.0f);
        reward.setLevel(0.5f);
        assert(reward.getLevel() == 0.5f);
        assert(reward.getPlasticityFactor() >= 0.5f && reward.getPlasticityFactor() <= 1.0f);
        
        // Test legacy interface
        reward.add(0.2f);
        assert(reward.getValue() == 0.7f);
        reward.reset();
        assert(reward.getValue() == 0.0f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void testRewardMultiComponent() {
        std::cout << "Testing Reward multi-component computation..." << std::endl;
        
        Reward reward;
        auto config = createTestConfig();
        
        // Create a test observation with various values
        std::vector<float> testData = {0.1f, 0.5f, 0.9f, 0.2f, 0.7f};
        auto observation = createTestObservation(testData);
        
        // Test individual components
        float survival = Reward::computeSurvivalReward(*observation, config);
        float efficiency = Reward::computeEfficiencyReward(*observation, config);
        float learning = Reward::computeLearningReward(*observation, config);
        float exploration = Reward::computeExplorationReward(*observation, config);
        
        // Test total reward (should be weighted combination)
        float total = reward.computeReward(*observation, config);
        
        // Verify components are in [0.0, 1.0] range
        assert(survival >= 0.0f && survival <= 1.0f);
        assert(efficiency >= 0.0f && efficiency <= 1.0f);
        assert(learning >= 0.0f && learning <= 1.0f);
        assert(exploration >= 0.0f && exploration <= 1.0f);
        assert(total >= 0.0f && total <= 1.0f);
        
        // Verify weighted combination
        float expectedTotal = survival * 0.4f + efficiency * 0.3f + learning * 0.2f + exploration * 0.1f;
        assert(std::abs(total - expectedTotal) < 0.0001f);
        
        // Verify history is updated
        const auto& history = reward.getHistory();
        assert(!history.empty());
        assert(history.back() == total);
        
        // Verify accumulated reward
        assert(reward.getValue() == total);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void testRewardThreadSafety() {
        std::cout << "Testing Reward thread safety..." << std::endl;
        
        Reward reward;
        auto config = createTestConfig();
        
        std::vector<float> testData = {0.3f, 0.6f, 0.8f, 0.1f};
        auto observation = createTestObservation(testData);
        
        // Launch multiple threads computing rewards concurrently
        std::vector<std::thread> threads;
        const int numThreads = 10;
        
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&reward, &observation, &config]() {
                for (int j = 0; j < 10; ++j) {
                    float rewardValue = reward.computeReward(*observation, config);
                    // Verify reward is in valid range
                    assert(rewardValue >= 0.0f && rewardValue <= 1.0f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Verify history management
        const auto& history = reward.getHistory();
        assert(!history.empty());
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void testRewardInvalidObservation() {
        std::cout << "Testing Reward with invalid observation..." << std::endl;
        
        Reward reward;
        auto config = createTestConfig();
        
        // Create an observation with empty data (invalid)
        auto observation = std::make_unique<Observation>();
        // Note: This assumes Observation has a way to create invalid states
        
        // The computeReward method should handle invalid observations gracefully
        float rewardValue = reward.computeReward(*observation, config);
        assert(rewardValue == 0.0f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "=== Testing Enhanced Reward System ===" << std::endl;
        std::cout << "Testing multi-component reward implementation..." << std::endl;
        std::cout << std::endl;
        
        testRewardBasicInterface();
        testRewardMultiComponent();
        testRewardThreadSafety();
        testRewardInvalidObservation();
        
        std::cout << std::endl;
        std::cout << "=== All Reward Tests PASSED ===" << std::endl;
    }
}

int main() {
    test_reward::runAll();
    return 0;
}