// BCM (Bienenstock-Cooper-Munro) Tests
#include "brain/Synapse.hpp"
#include "plasticity/BCMRule.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_bcm {

void testBCMCreation() {
    nlm::BCMRule rule;
    
    assert(rule.getLearningRate() == 0.001f);
    assert(rule.getHebbianThreshold() == 0.5f);
    assert(rule.getHomeostaticTarget() == 0.1f);
    assert(rule.getMinWeight() == -0.5f);
    assert(rule.getMaxWeight() == 1.0f);
    
    std::cout << "    testBCMCreation passed" << std::endl;
}

void testBCMConfiguration() {
    nlm::BCMRule rule;
    
    rule.setLearningRate(0.005f);
    rule.setHebbianThreshold(0.3f);
    rule.setHomeostaticTarget(0.05f);
    
    assert(rule.getLearningRate() == 0.005f);
    assert(rule.getHebbianThreshold() == 0.3f);
    assert(rule.getHomeostaticTarget() == 0.05f);
    
    std::cout << "    testBCMConfiguration passed" << std::endl;
}

void testBCMName() {
    nlm::BCMRule rule;
    
    assert(rule.getName() != nullptr);
    assert(std::string(rule.getName()) == "BCM");
    
    std::cout << "    testBCMName passed" << std::endl;
}

void testBCMLearning() {
    nlm::BCMRule rule;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Simulate learning with different activity patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed based on BCM rule
    assert(std::abs(finalWeight - initialWeight) > 1e-5f);
    
    std::cout << "    testBCMLearning passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMLearningStabilization() {
    nlm::BCMRule rule;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Apply learning multiple times
    for (int i = 0; i < 10; ++i) {
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
        rule.update(&synapse, preSpikes, postSpikes, 0.001);
    }
    
    // Weight should stabilize (not go outside bounds)
    assert(synapse.getWeight() <= 1.0f);
    assert(synapse.getWeight() >= -0.5f);
    
    std::cout << "    testBCMLearningStabilization passed" << std::endl;
}

void runAll() {
    std::cout << "Running BCM tests..." << std::endl;
    testBCMCreation();
    testBCMConfiguration();
    testBCMName();
    testBCMLearning();
    testBCMLearningStabilization();
}

} // namespace test_bcm