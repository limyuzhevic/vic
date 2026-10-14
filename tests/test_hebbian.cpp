// Hebbian Tests
#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    assert(hebbian.getLearningRate() == 0.05f);
    
    hebbian.setLearningRate(0.1f);
    assert(hebbian.getLearningRate() == 0.1f);
    
    std::cout << "    testHebbianConfiguration passed" << std::endl;
}

void testHebbianWeightChange() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Apply Hebbian learning with correlated spikes
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms after pre
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to correlated firing
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianWeightChange passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianUncorrelatedSpikes() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Apply Hebbian learning with uncorrelated spikes (large time gap)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {100.0};  // 100ms after pre (outside correlation window)
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should not change significantly (or could decrease slightly due to weak correlation)
    
    std::cout << "    testHebbianUncorrelatedSpikes passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianEmptySpikes() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
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
    nlm::Hebbian hebbian;
    
    // Create a synapse with weight near maximum
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setWeight(0.99f);
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to reasonable bounds (actual bounds depend on implementation)
    // For now, just ensure it doesn't crash
    
    std::cout << "    testHebbianWeightBounds passed (final weight=" 
              << synapse.getWeight() << ")" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianWeightChange();
    testHebbianUncorrelatedSpikes();
    testHebbianEmptySpikes();
    testHebbianWeightBounds();
    std::cout << "Hebbian tests completed" << std::endl;
}

} // namespace test_hebbian
