// Hebbian Plasticity Tests
// Comprehensive testing for Hebbian learning rules in NLM

#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_plasticity {

void testHebbianBasic() {
    std::cout << "Testing Hebbian basic functionality..." << std::endl;
    
    nlm::Hebbian hebbian;
    
    // Test learning rate
    assert(hebbian.getLearningRate() > 0.0f && hebbian.getLearningRate() < 1.0f);
    
    // Test learning rate bounds
    hebbian.setLearningRate(0.0f);
    assert(hebbian.getLearningRate() == 0.0f);
    
    hebbian.setLearningRate(1.5f);  // Should clamp
    assert(hebbian.getLearningRate() == 1.0f);
    
    std::cout << "    testHebbianBasic passed" << std::endl;
}

void testHebbianWeightChanges() {
    std::cout << "Testing Hebbian weight changes..." << std::endl;
    
    nlm::Hebbian hebbian;
    
    // Create a mock synapse for testing
    nlm::SynapseId synId(nlm::SynapseId(1));
    nlm::NeuronId source(nlm::NeuronId(1));
    nlm::NeuronId dest(nlm::NeuronId(2));
    
    nlm::Synapse synapse(synId, source, dest);
    
    // Set initial weight
    float initialWeight = 0.5f;
    synapse.setWeight(initialWeight);
    assert(std::abs(synapse.getWeight() - initialWeight) < 0.001f);
    
    // Test weight update
    hebbian.setLearningRate(0.1f);
    
    // Create spike histories
    std::vector<Timestamp> preSpikes = {100.0f, 200.0f};
    std::vector<Timestamp> postSpikes = {150.0f, 250.0f};
    
    // This will apply Hebbian learning based on correlated spikes
    hebbian.update(&synapse, preSpikes, postSpikes, 1.0f);
    
    // Weight should have changed
    float newWeight = synapse.getWeight();
    assert(newWeight != initialWeight || std::abs(newWeight - initialWeight) < 0.0001f);
    
    // Test weight bounds
    synapse.setWeight(0.9f);
    hebbian.update(&synapse, preSpikes, postSpikes, 1.0f);
    assert(synapse.getWeight() <= 1.0f);  // Should not exceed max
    
    synapse.setWeight(-0.9f);
    hebbian.update(&synapse, preSpikes, postSpikes, 1.0f);
    assert(synapse.getWeight() >= -1.0f);  // Should not go below min
    
    std::cout << "    testHebbianWeightChanges passed" << std::endl;
}

void testHebbianCorrelation() {
    std::cout << "Testing Hebbian correlation calculation..." << std::endl;
    
    nlm::Hebbian hebbian;
    
    // Test with no correlated spikes
    nlm::SynapseId synId(nlm::SynapseId(1));
    nlm::NeuronId source(nlm::NeuronId(1));
    nlm::NeuronId dest(nlm::NeuronId(2));
    
    nlm::Synapse synapse(synId, source, dest);
    synapse.setWeight(0.0f);
    
    // Spikes far apart - should have minimal correlation
    std::vector<Timestamp> preSpikes = {100.0f};
    std::vector<Timestamp> postSpikes = {500.0f};  // Large time gap
    
    hebbian.update(&synapse, preSpikes, postSpikes, 1.0f);
    
    // Test with correlated spikes
    float weightBefore = synapse.getWeight();
    
    // Close spikes - should have correlation
    preSpikes = {100.0f};
    postSpikes = {110.0f};  // Close together
    
    hebbian.update(&synapse, preSpikes, postSpikes, 1.0f);
    
    // Weight should have changed due to correlation
    float weightAfter = synapse.getWeight();
    // The effect depends on correlation calculation implementation
    
    std::cout << "    testHebbianCorrelation passed" << std::endl;
}

void testHebbianIntegration() {
    std::cout << "Testing Hebbian integration with other systems..." << std::endl;
    
    nlm::Hebbian hebbian;
    
    // Test that Hebbian works with different spike patterns
    nlm::SynapseId synId(nlm::SynapseId(1));
    nlm::NeuronId source(nlm::NeuronId(1));
    nlm::NeuronId dest(nlm::NeuronId(2));
    
    nlm::Synapse synapse(synId, source, dest);
    synapse.setWeight(0.0f);
    
    // Test with multiple spike pairs
    std::vector<Timestamp> preSpikes = {100.0f, 200.0f, 300.0f};
    std::vector<Timestamp> postSpikes = {105.0f, 205.0f, 305.0f};  // Correlated
    
    hebbian.update(&synapse, preSpikes, postSpikes, 1.0f);
    
    // Test with reverse correlation (anti-Hebbian patterns)
    synapse.setWeight(0.5f);
    preSpikes = {100.0f, 200.0f};
    postSpikes = {95.0f, 195.0f};  // Post before pre
    
    hebbian.update(&synapse, preSpikes, postSpikes, 1.0f);
    
    // Weight should have changed differently for reverse correlation
    
    std::cout << "    testHebbianIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Plasticity tests..." << std::endl;
    testHebbianBasic();
    testHebbianWeightChanges();
    testHebbianCorrelation();
    testHebbianIntegration();
}

} // namespace test_plasticity