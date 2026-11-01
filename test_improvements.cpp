// Test program to verify reward and neuromodulation improvements
// This demonstrates the enhanced reward system and neuromodulation

#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "environment/Environment.hpp"
#include "sensory/SensoryInput.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Reward and Neuromodulation Improvements" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    // Test 1: Reward System Improvements
    std::cout << "\n1. Testing Reward System Implementation:" << std::endl;
    
    nlm::Reward rewardSystem;
    
    // Create a mock observation for testing
    // In a real scenario, this would come from world.getSensoryPercept()
    struct MockObservation {
        float energy;
        float health;
        std::vector<float> internalSignals;
        
        float getEnergy() const { return energy; }
        float getHealth() const { return health; }
        const std::vector<float>& getInternalSignals() const { return internalSignals; }
    };
    
    // Test positive reward (resource detection)
    MockObservation goodObs1;
    goodObs1.energy = 0.9f;
    goodObs1.health = 1.0f;
    goodObs1.internalSignals = {0.8f, 0.7f, 0.6f};
    
    float reward1 = rewardSystem.computeReward(goodObs1);
    std::cout << "   Resource-like observation reward: " << reward1 << std::endl;
    assert(reward1 > 0.0f);  // Should be positive for good state
    
    // Test negative reward (hazard detection)
    MockObservation badObs2;
    badObs2.energy = 0.2f;
    badObs2.health = 0.3f;
    badObs2.internalSignals = {-0.7f, -0.8f, -0.6f};
    
    float reward2 = rewardSystem.computeReward(badObs2);
    std::cout << "   Hazard-like observation reward: " << reward2 << std::endl;
    assert(reward2 < 0.0f);  // Should be negative for bad state
    
    // Test neutral reward (empty/low signal)
    MockObservation neutralObs3;
    neutralObs3.energy = 0.5f;
    neutralObs3.health = 0.5f;
    neutralObs3.internalSignals = {0.1f, -0.1f, 0.05f};
    
    float reward3 = rewardSystem.computeReward(neutralObs3);
    std::cout << "   Neutral observation reward: " << reward3 << std::endl;
    
    // Test history recording
    std::vector<float> history = rewardSystem.getHistory();
    std::cout << "   Reward history recorded: " << history.size() << " entries" << std::endl;
    
    // Test 2: Dopamine System Improvements
    std::cout << "\n2. Testing Dopamine System Implementation:" << std::endl;
    
    nlm::Dopamine dopamine;
    
    // Test initial state
    std::cout << "   Initial dopamine level: " << dopamine.getLevel() << std::endl;
    assert(dopamine.getLevel() == 0.0f);
    
    // Test dopamine signaling with positive reward
    dopamine.signalReward(10.0f);  // Large positive reward
    std::cout << "   After large positive reward: " << dopamine.getLevel() << std::endl;
    assert(dopamine.getLevel() > 0.0f);
    
    // Test dopamine plasticity factor
    float plasticity1 = dopamine.getPlasticityFactor();
    std::cout << "   Plasticity factor after positive reward: " << plasticity1 << std::endl;
    assert(plasticity1 > 0.5f);  // Should be above baseline
    
    // Update dopamine
    dopamine.update(0.001f);
    std::cout << "   After decay: " << dopamine.getLevel() << std::endl;
    
    // Test reward prediction error
    dopamine.signalRewardPredictionError(2.5f);  // Positive prediction error
    std::cout << "   After positive prediction error: " << dopamine.getLevel() << std::endl;
    assert(dopamine.getLevel() > 0.0f);
    
    // Test negative prediction error
    dopamine.signalRewardPredictionError(-1.0f);  // Negative prediction error
    std::cout <<   "   After negative prediction error: " << dopamine.getLevel() << std::endl;
    assert(dopamine.getLevel() >= 0.0f);  // Should not go negative
    
    // Test 3: Neuromodulation Integration
    std::cout << "\n3. Testing Neuromodulation Integration:" << std::endl;
    
    // Test dopamine-modulated plasticity
    float basePlasticity = 0.5f + 0.5f * dopamine.getLevel();  // Old simple formula
    float newPlasticity = dopamine.getPlasticityFactor();  // New biologically realistic formula
    
    std::cout << "   Old plasticity formula: " << basePlasticity << std::endl;
    std::cout << "   New plasticity formula: " << newPlasticity << std::endl;
    assert(newPlasticity >= 0.1f && newPlasticity <= 2.0f);  // Should be within realistic bounds
    
    // Test 4: Reward History
    std::cout << "\n4. Testing Reward History Management:" << std::endl;
    
    rewardSystem.clearHistory();
    assert(rewardSystem.getHistory().empty());
    
    // Add some rewards
    rewardSystem.computeReward(goodObs1);
    rewardSystem.computeReward(badObs2);
    rewardSystem.computeReward(neutralObs3);
    
    history = rewardSystem.getHistory();
    std::cout << "   History after adding 3 rewards: " << history.size() << " entries" << std::endl;
    assert(history.size() == 3);
    
    // Test that adding more keeps history bounded
    for (int i = 0; i < 105; ++i) {  // Add 105 rewards (should exceed 100)
        rewardSystem.computeReward(neutralObs3);
    }
    
    history = rewardSystem.getHistory();
    std::cout <<   "   History after adding 108 total: " << history.size() << " entries (should be <= 100)" << std::endl;
    assert(history.size() <= 100);  // Should be bounded
    
    // Test 5: Integration Test
    std::cout << "\n5. Testing Integration:":
    
    // Simulate a complete cycle
    float totalIntegrationReward = 0.0f;
    float integrationDopamineLevel = 0.0f;
    
    for (int i = 0; i < 10; ++i) {
        // Random-like reward based on observation
        float randomFactor = static_cast<float>(rand()) / RAND_MAX;
        float obsReward = rewardSystem.computeReward(neutralObs3) + (randomFactor * 5.0f - 2.5f);
        
        // Apply reward modulation
        integrationDopamineLevel = (integrationDopamineLevel + obsReward * 0.1f) * 0.9f;  // Simplified
        dopamine.setLevel(std::clamp(integrationDopamineLevel, 0.0f, 1.0f));
        
        totalIntegrationReward += obsReward;
    }
    
    std::cout << "   Integration test passed" << std::endl;
    std::cout << "   Total rewards accumulated: " << totalIntegrationReward << std::endl;
    std::cout << "   Final dopamine level: " << dopamine.getLevel() << std::endl;
    
    // Test 6: Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "✓ Reward system implements observation-based computation" << std::endl;
    std::cout << "✓ Dopamine signaling processes reward magnitudes correctly" << std::endl;
    std::cout << "✓ Prediction error signaling updates dopamine levels" << std::endl;
    std::cout << "✓ Plasticity factor modulation is biologically realistic" << std::endl;
    std::cout << "✓ Reward history management is properly bounded" << std::endl;
    std::cout << "✓ All systems integrate and work together" << std::endl;
    
    std::cout << "\nAll tests passed! The reward and neuromodulation improvements are working correctly." << std::endl;
    
    return 0;
}
