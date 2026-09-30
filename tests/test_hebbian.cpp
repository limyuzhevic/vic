// Hebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include <cassert>
#include <iostream>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 2.0f);
    
    std::cout << "    testHebbianConfiguration passed" << std::endl;
}

void testHebbianName() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "    testHebbianName passed" << std::endl;
}

void testHebbianPotentiation() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre and post fire in correlation (within 100ms window)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // 5ms offset
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to correlated firing
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianNoCorrelatedSpikes() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre and post fire far apart (no correlation)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {200.0};  // 200ms offset > 100ms window
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should remain unchanged
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testHebbianNoCorrelatedSpikes passed" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::Hebbian hebbian;
    
    // Create a synapse near max weight
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.95f);
    
    // Create strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianName();
    testHebbianPotentiation();
    testHebbianNoCorrelatedSpikes();
    testHebbianWeightBounds();
}

} // namespace test_hebbian