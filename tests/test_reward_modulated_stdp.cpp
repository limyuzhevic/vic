// RewardModulatedSTDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_reward_modulated_stdp {

void testRSTPDCreation() {
    nlm::RewardModulatedSTDP rstdp;
    
    assert(rstdp.getLTPWeight() == 0.01f);
    assert(rstdp.getLTDWeight() == 0.012f);
    assert(rstdp.getTimeConstant() == 20.0f);
    assert(rstdp.getRewardFactor() == 1.0f);
    assert(rstdp.getTraceDecayRate() == 0.01f);
    assert(rstdp.getTraceBaseline() == 0.0f);
    
    std::cout << "    testRSTPDCreation passed" << std::endl;
}

void testRSTDPConfiguration() {
    nlm::RewardModulatedSTDP rstdp;
    
    rstdp.setLTPWeight(0.05f);
    rstdp.setLTDWeight(0.06f);
    rstdp.setTimeConstant(30.0f);
    rstdp.setRewardFactor(2.0f);
    rstdp.setTraceDecayRate(0.05f);
    rstdp.setTraceBaseline(0.1f);
    
    assert(rstdp.getLTPWeight() == 0.05f);
    assert(rstdp.getLTDWeight() == 0.06f);
    assert(rstdp.getTimeConstant() == 30.0f);
    assert(rstdp.getRewardFactor() == 2.0f);
    assert(rstdp.getTraceDecayRate() == 0.05f);
    assert(rstdp.getTraceBaseline() == 0.1f);
    
    std::cout << "    testRSTDPConfiguration passed" << std::endl;
}

void testRSTDPConfigure() {
    nlm::RewardModulatedSTDP rstdp;
    
    rstdp.configure(0.03f, 0.035f, 25.0f, 1.5f, 0.02f);
    
    assert(rstdp.getLTPWeight() == 0.03f);
    assert(rstdp.getLTDWeight() == 0.035f);
    assert(rstdp.getTimeConstant() == 25.0f);
    assert(rstdp.getRewardFactor() == 1.5f);
    assert(rstdp.getTraceDecayRate() == 0.02f);
    
    std::cout << "    testRSTDPConfigure passed" << std::endl;
}

void testRSTDPName() {
    nlm::RewardModulatedSTDP rstdp;
    
    assert(rstdp.getName() != nullptr);
    assert(std::string(rstdp.getName()) == "R-STDP");
    
    std::cout << "    testRSTDPName passed" << std::endl;
}

void testRSTDPWeightBounds() {
    nlm::RewardModulatedSTDP rstdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    // Test minimum weight
    synapse.setWeight(-0.99f);  // Near min
    preSpikes = {10.0, 11.0, 12.0, 13.0, 14.0};
    postSpikes = {5.0, 6.0, 7.0, 8.0, 9.0};  // Post before pre
    
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to min
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testRSTDPWeightBounds passed" << std::endl;
}

void testRSTDPRewardFactor() {
    nlm::RewardModulatedSTDP rstdp;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Build eligibility trace
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Apply reward with different factors
    rstdp.setRewardFactor(1.0f);
    rstdp.applyReward(&synapse, 1.0f);
    
    float weightWithFactor1 = synapse.getWeight();
    
    // Reset and test with higher reward factor
    synapse.setWeight(0.5f);
    synapse.clearHistory();
    synapse.setEligibilityTrace(0.0f);
    
    rstdp.setRewardFactor(2.0f);
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    rstdp.applyReward(&synapse, 1.0f);
    
    float weightWithFactor2 = synapse.getWeight();
    
    // Weight change should be proportional to reward factor
    assert(weightWithFactor2 > weightWithFactor1);
    
    std::cout << "    testRSTDPRewardFactor passed (Δ1=" 
              << (weightWithFactor1 - 0.5f) << ", Δ2=" 
              << (weightWithFactor2 - 0.5f) << ")" << std::endl;
}

void testRSTDPTraceDecay() {
    nlm::RewardModulatedSTDP rstdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    
    // Set initial trace
    float initialTrace = 0.5f;
    synapse.setEligibilityTrace(initialTrace);
    
    // Apply decay
    rstdp.decayTrace(&synapse, 100.0);  // 100ms decay
    
    float decayedTrace = synapse.getEligibilityTrace();
    
    // Trace should be less than initial
    assert(decayedTrace < initialTrace);
    
    // Test with lower decay rate
    nlm::RewardModulatedSTDP rstdpLowDecay;
    rstdpLowDecay.setTraceDecayRate(0.001f);  // Very low decay
    
    synapse.setEligibilityTrace(initialTrace);
    rstdpLowDecay.decayTrace(&synapse, 100.0);
    
    float decayedTraceLow = synapse.getEligibilityTrace();
    
    // Trace should be closer to initial with low decay
    assert(decayedTraceLow > decayedTrace);
    
    std::cout << "    testRSTDPTraceDecay passed (initial=1.0, decay=0.01, decayed=" 
              << decayedTrace << ", decay=0.001, decayed=" << decayedTraceLow << ")" << std::endl;
}

void runAll() {
    std::cout << "Running RewardModulatedSTDP tests..." << std::endl;
    testRSTPDCreation();
    testRSTDPConfiguration();
    testRSTDPConfigure();
    testRSTDPName();
    testRSTDPWeightBounds();
    testRSTDPRewardFactor();
    testRSTDPTraceDecay();
}

} // namespace test_reward_modulated_stdp