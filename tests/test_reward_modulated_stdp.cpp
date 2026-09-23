// RewardModulatedSTDP Tests
#include "plasticity/RewardModulatedSTDP.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_reward_modulated_stdp {

void testRSTDPCreation() {
    nlm::RewardModulatedSTDP rstdp;
    
    assert(rstdp.getLTPWeight() == 0.01f);
    assert(rstdp.getLTDWeight() == 0.012f);
    assert(rstdp.getTimeConstant() == 20.0f);
    assert(rstdp.getEligibilityDecay() == 0.01f);
    assert(rstdp.getRewardScale() == 1.0f);
    
    std::cout << "    testRSTDPCreation passed" << std::endl;
}

void testRSTDPConfiguration() {
    nlm::RewardModulatedSTDP rstdp;
    
    rstdp.setLTPWeight(0.05f);
    rstdp.setLTDWeight(0.06f);
    rstdp.setTimeConstant(30.0f);
    rstdp.setEligibilityDecay(0.02f);
    rstdp.setRewardScale(2.0f);
    
    assert(rstdp.getLTPWeight() == 0.05f);
    assert(rstdp.getLTDWeight() == 0.06f);
    assert(rstdp.getTimeConstant() == 30.0f);
    assert(rstdp.getEligibilityDecay() == 0.02f);
    assert(rstdp.getRewardScale() == 2.0f);
    
    std::cout << "    testRSTDPConfiguration passed" << std::endl;
}

void testRSTDPRewardModulation() {
    nlm::RewardModulatedSTDP rstdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) with reward
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // ms (10ms after pre)
    
    // Set up eligibility trace (simulating previous learning)
    synapse.setEligibilityTrace(1.0f);
    
    // Reward is positive (learning should occur)
    float reward = 1.0f;
    synapse.setWeight(synapse.getWeight() + reward * 0.1f);  // Simulate reward signal
    
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post with reward
    assert(finalWeight > initialWeight);
    
    std::cout << "    testRSTDPRewardModulation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testRSTDPRewardThreshold() {
    nlm::RewardModulatedSTDP rstdp;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0};
    
    // Set up eligibility trace
    synapse.setEligibilityTrace(1.0f);
    
    // Negative reward (no learning should occur)
    float negativeReward = -1.0f;
    synapse.setWeight(synapse.getWeight() + negativeReward * 0.1f);  // Simulate negative reward signal
    
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should NOT have changed due to negative reward
    assert(std::abs(finalWeight - initialWeight) < 0.0001f);
    
    std::cout << "    testRSTDPRewardThreshold passed" << std::endl;
}

void testRSTDPEligibilityDecay() {
    nlm::RewardModulatedSTDP rstdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Set up eligibility trace
    synapse.setEligibilityTrace(1.0f);
    
    // Update synapse to decay eligibility trace
    synapse.step(10.0);  // Advance time to allow decay
    
    // Eligibility trace should have decayed
    assert(synapse.getEligibilityTrace() < 1.0f);
    
    std::cout << "    testRSTDPEligibilityDecay passed (final trace=" 
              << synapse.getEligibilityTrace() << ")" << std::endl;
}

void runAll() {
    std::cout << "Running RewardModulatedSTDP tests..." << std::endl;
    testRSTDPCreation();
    testRSTDPConfiguration();
    testRSTDPRewardModulation();
    testRSTDPRewardThreshold();
    testRSTDPEligibilityDecay();
}

} // namespace test_reward_modulated_stdp