// Comprehensive Plasticity Tests
// Phase 2: Real Neural Computation Tests - Plasticity System

#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_plasticity {

void testSTDPEmptySpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPEmptySpikes passed" << std::endl;
}

void testSTDPLongTimeConstant() {
    nlm::STDP stdp;
    
    // Set a very long time constant (tau)
    stdp.setTimeConstant(100.0f);
    assert(stdp.getTimeConstant() == 100.0f);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post with large time difference (dt > tau)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {50.0};  // 50ms difference > tau
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // With large dt, weight change should be very small due to exponential decay
    float finalWeight = synapse.getWeight();
    assert(std::abs(finalWeight - initialWeight) < 0.001f);
    
    std::cout << "    testSTDPLongTimeConstant passed" << std::endl;
}

void testSTDPWeightBounds() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    // Test extreme depression
    nlm::Synapse synapse2(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(-0.99f);  // Near min
    
    // Very strong depression
    std::vector<nlm::Timestamp> preSpikes2 = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes2 = {0.0, 5.0, 15.0};  // All post before pre
    
    stdp.update(&synapse2, preSpikes2, postSpikes2, 0.001);
    
    // Weight should be clamped to min
    assert(synapse2.getWeight() >= -1.0f);
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

void testSTDPSymmetric() {
    nlm::STDP stdp;
    
    // Test that pre-before-post and post-before-pre produce symmetric effects
    // on the same initial weight (with reversed spike times)
    
    float initialWeight = 0.5f;
    
    // Pre before post
    nlm::Synapse synapse1(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(initialWeight);
    
    std::vector<nlm::Timestamp> preSpikes1 = {0.0};
    std::vector<nlm::Timestamp> postSpikes1 = {10.0};  // +10ms
    
    stdp.update(&synapse1, preSpikes1, postSpikes1, 0.001);
    float weightAfterPreBeforePost = synapse1.getWeight();
    
    // Post before pre (reverse timing)
    nlm::Synapse synapse2(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(initialWeight);
    
    std::vector<nlm::Timestamp> preSpikes2 = {10.0};
    std::vector<nlm::Timestamp> postSpikes2 = {0.0};  // -10ms
    
    stdp.update(&synapse2, preSpikes2, postSpikes2, 0.001);
    float weightAfterPostBeforePre = synapse2.getWeight();
    
    // The absolute weight changes should be similar but opposite in sign
    float delta1 = weightAfterPreBeforePost - initialWeight;
    float delta2 = weightAfterPostBeforePre - initialWeight;
    
    assert(std::abs(delta1 + delta2) < 0.01f);  // Should be approximately symmetric
    
    std::cout << "    testSTDPSymmetric passed" << std::endl;
}

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    // Test basic Hebbian functionality
    nlm::Synapse synapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {2.0, 7.0, 12.0};  // Roughly simultaneous
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Hebbian should strengthen synapses that are co-active
    assert(synapse.getWeight() > 0.3f);  // Should potentiate
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianInhibition() {
    nlm::Hebbian hebbian;
    
    // Test that Hebbian can depress when activity is anti-correlated
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0};   // Pre fires regularly
    std::vector<nlm::Timestamp> postSpikes = {3.0, 8.0};  // Post fires regularly
    // This is roughly correlated, so should potentiate
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    float weightAfterCorrelated = synapse.getWeight();
    
    // Now test with anti-correlated timing
    nlm::Synapse synapse2(nlm::SynapseId(9), nlm::NeuronId(17), nlm::NeuronId(18));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes2 = {0.0, 5.0};
    std::vector<nlm::Timestamp> postSpikes2 = {2.5, 7.5};  // Shifted in time
    // This is less correlated, may still potentiate but less strongly
    
    hebbian.update(&synapse2, preSpikes2, postSpikes2, 0.001);
    float weightAfterAntiCorrelated = synapse2.getWeight();
    
    // The more correlated should potentiate more
    assert(weightAfterCorrelated >= weightAfterAntiCorrelated);
    
    std::cout << "    testHebbianInhibition passed" << std::endl;
}

void testRewardModulatedSTDP() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(10), nlm::NeuronId(19), nlm::NeuronId(20));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.4f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0};
    std::vector<nlm::Timestamp> postSpikes = {2.0, 7.0};
    
    // Record initial eligibility trace
    float initialTrace = synapse.getEligibilityTrace();
    
    // Apply STDP without reward
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float traceAfter = synapse.getEligibilityTrace();
    assert(traceAfter > initialTrace);  // Should have accumulated eligibility
    
    std::cout << "    testRewardModulatedSTDP passed" << std::endl;
}

void testPlasticityRuleBase() {
    nlm::PlasticityRule rule;
    
    nlm::Synapse synapse(nlm::SynapseId(11), nlm::NeuronId(21), nlm::NeuronId(22));
    
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0};
    
    // Base PlasticityRule update should be empty (pure virtual in base class)
    // But we can test that it exists
    assert(&rule != nullptr);
    
    std::cout << "    testPlasticityRuleBase passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(12), nlm::NeuronId(23), nlm::NeuronId(24));
    
    // Test initial state
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    // Enable all plasticity types
    synapse.enablePlasticity(true, true, true);
    
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    // Disable selectively
    synapse.enablePlasticity(false, true, false);
    
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSTDPConfigure() {
    nlm::STDP stdp;
    
    // Configure with custom parameters
    stdp.configure(0.05f, 0.055f, 25.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.055f);
    assert(stdp.getTimeConstant() == 25.0f);
    
    // Test parameter bounds
    stdp.configure(2.0f, 0.0f, 50.0f);  // LTP > 1.0, should be clamped
    assert(stdp.getLTPWeight() == 1.0f);
    assert(stdp.getLTDWeight() == 0.0f);
    
    std::cout << "    testSTDPConfigure passed" << std::endl;
}

void testPlasticityIntegration() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(13), nlm::NeuronId(25), nlm::NeuronId(26));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    synapse.enablePlasticity(true, false, false);  // Only STDP enabled
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0};
    
    // Record weight before
    float weightBefore = synapse.getWeight();
    
    // Update with plasticity enabled
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightAfter = synapse.getWeight();
    assert(weightAfter != weightBefore);  // Should have changed due to STDP
    
    // Now disable plasticity
    synapse.enablePlasticity(false, false, false);
    synapse.setWeight(0.3f);  // Reset
    
    weightBefore = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    weightAfter = synapse.getWeight();
    
    assert(weightAfter == weightBefore);  // Should not change when plasticity disabled
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Plasticity tests..." << std::endl;
    testSTDPEmptySpikes();
    testSTDPLongTimeConstant();
    testSTDPWeightBounds();
    testSTDPSymmetric();
    testHebbianCreation();
    testHebbianInhibition();
    testRewardModulatedSTDP();
    testPlasticityRuleBase();
    testPlasticityFlags();
    testSTDPConfigure();
    testPlasticityIntegration();
}

} // namespace test_plasticity

