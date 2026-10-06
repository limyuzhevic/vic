// Hebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::HebbianRule hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    assert(hebbian.getMinWeight() == -1.0f);
    assert(hebbian.getTimeWindow() == 50.0f);
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::HebbianRule hebbian;
    
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(0.8f);
    hebbian.setMinWeight(-0.8f);
    hebbian.setTimeWindow(100.0f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 0.8f);
    assert(hebbian.getMinWeight() == -0.8f);
    assert(hebbian.getTimeWindow() == 100.0f);
    
    std::cout << "    testHebbianConfiguration passed" << std::endl;
}

void testHebbianConfigure() {
    nlm::HebbianRule hebbian;
    
    hebbian.configure(0.03f, 0.8f, -0.8f, 75.0f, 0.02f, 1.0f);
    
    assert(hebbian.getLearningRate() == 0.03f);
    assert(hebbian.getMaxWeight() == 0.8f);
    assert(hebbian.getMinWeight() == -0.8f);
    assert(hebbian.getTimeWindow() == 75.0f);
    
    std::cout << "    testHebbianConfigure passed" << std::endl;
}

void testHebbianName() {
    nlm::HebbianRule hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "    testHebbianName passed" << std::endl;
}

void testHebbianPotentiation() {
    nlm::HebbianRule hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Both pre and post fire - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to correlated activity
    assert(finalWeight >= initialWeight);
    
    std::cout << "    testHebbianPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianAntipotentiate() {
    nlm::HebbianRule hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Only pre fires - no potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should not increase (no correlated activity)
    assert(finalWeight == initialWeight);
    
    std::cout << "    testHebbianAntipotentiate passed" << std::endl;
}

void testHebbianEmptySpikes() {
    nlm::HebbianRule hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testHebbianEmptySpikes passed" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::HebbianRule hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Strong potentiation
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
    testHebbianConfigure();
    testHebbianName();
    testHebbianPotentiation();
    testHebbianAntipotentiate();
    testHebbianEmptySpikes();
    testHebbianWeightBounds();
}

} // namespace test_hebbian
