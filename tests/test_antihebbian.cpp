// AntiHebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/AntiHebbianRule.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_antihebbian {

void testAntiHebbianCreation() {
    nlm::AntiHebbianRule rule;
    
    assert(rule.getLearningRate() == 0.01f);
    assert(rule.getMinWeight() == -1.0f);
    assert(rule.getMaxWeight() == 0.0f);
    assert(rule.getTargetWeight() == 0.0f);
    
    std::cout << "    testAntiHebbianCreation passed" << std::endl;
}

void testAntiHebbianConfiguration() {
    nlm::AntiHebbianRule rule;
    
    rule.setLearningRate(0.05f);
    rule.setMinWeight(-0.5f);
    rule.setMaxWeight(-0.1f);
    rule.setTargetWeight(-0.05f);
    
    assert(rule.getLearningRate() == 0.05f);
    assert(rule.getMinWeight() == -0.5f);
    assert(rule.getMaxWeight() == -0.1f);
    assert(rule.getTargetWeight() == -0.05f);
    
    std::cout << "    testAntiHebbianConfiguration passed" << std::endl;
}

void testAntiHebbianName() {
    nlm::AntiHebbianRule rule;
    
    assert(rule.getName() != nullptr);
    assert(std::string(rule.getName()) == "AntiHebbian");
    
    std::cout << "    testAntiHebbianName passed" << std::endl;
}

void testAntiHebbianPotentiation() {
    nlm::AntiHebbianRule rule;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.8f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires after post (negative dt) - should depress (anti-hebbian)
    std::vector<nlm::Timestamp> preSpikes = {5.0, 15.0, 25.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {0.0, 10.0, 20.0};  // ms (before pre)
    
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre (anti-hebbian)
    assert(finalWeight < initialWeight);
    
    std::cout << "    testAntiHebbianPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testAntiHebbianWeightBounds() {
    nlm::AntiHebbianRule rule;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(-0.05f);  // Near max (0.0)
    
    // Apply strong depression
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max (-0.1f in our test)
    assert(synapse.getWeight() <= -0.1f);
    assert(synapse.getWeight() >= -1.0f);  // min bound
    
    std::cout << "    testAntiHebbianWeightBounds passed" << std::endl;
}

void runAll() {
    std::cout << "Running AntiHebbian tests..." << std::endl;
    testAntiHebbianCreation();
    testAntiHebbianConfiguration();
    testAntiHebbianName();
    testAntiHebbianPotentiation();
    testAntiHebbianWeightBounds();
}

} // namespace test_antihebbian