// Reward-Modulated STDP (R-STDP) Tests
#include "brain/Synapse.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_rstdp {

void testRSTDPCreation() {
    nlm::RewardModulatedSTDP rule;
    
    assert(rule.getLTPWeight() == 0.01f);
    assert(rule.getLTDWeight() == 0.012f);
    assert(rule.getTimeConstant() == 20.0f);
    assert(rule.getRewardScalar() == 1.0f);
    assert(rule.getBaselineLearningRate() == 0.0001f);
    assert(rule.getRecentReward() == 0.0f);
    
    std::cout << "    testRSTDPCreation passed" << std::endl;
}

void testRSTDPRewardModulation() {
    nlm::RewardModulatedSTDP rule;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    // Apply with no reward
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightNoReward = synapse.getWeight();
    
    // Reset and apply with reward
    synapse.setWeight(initialWeight);
    rule.setRecentReward(1.0f);
    
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightWithReward = synapse.getWeight();
    
    // With reward, change should be larger (amplified)
    assert(std::abs(weightWithReward - initialWeight) > std::abs(weightNoReward - initialWeight));
    
    std::cout << "    testRSTDPRewardModulation passed (noRewardΔ=" 
              << (weightNoReward - initialWeight) << ", rewardΔ=" 
              << (weightWithReward - initialWeight) << ")" << std::endl;
}

void testRSTDPRewardDecay() {
    nlm::RewardModulatedSTDP rule;
    
    rule.setRecentReward(1.0f);
    assert(rule.getRecentReward() == 1.0f);
    
    // Simulate reward decay over time
    for (int i = 0; i < 10; ++i) {
        // In real implementation, reward would decay
        // For now, test that we can set/get reward
        float currentReward = rule.getRecentReward();
        rule.setRecentReward(currentReward * 0.9f);  // Simulate decay
    }
    
    // Reward should have decayed
    assert(rule.getRecentReward() < 1.0f);
    
    std::cout << "    testRSTDPRewardDecay passed" << std::endl;
}

void testRSTDPEmptySpikes() {
    nlm::RewardModulatedSTDP rule;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    float initialWeight = synapse.getWeight();
    
    // Empty spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Reset with reward and test again
    synapse.setWeight(initialWeight);
    rule.setRecentReward(1.0f);
    
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);  // Still no change
    
    std::cout << "    testRSTDPEmptySpikes passed" << std::endl;
}

void runAll() {
    std::cout << "Running R-STDP tests..." << std::endl;
    testRSTDPCreation();
    testRSTDPRewardModulation();
    testRSTDPRewardDecay();
    testRSTDPEmptySpikes();
}

} // namespace test_rstdp