// Hebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <stdexcept>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(0.9f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 0.9f);
    
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
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Both pre and post spikes at similar times - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 10.0, 20.0};  // Fixed: removed "Lee"
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to correlated activity
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianPotentiation passed (Δ="
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianDepression() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.8f);
    
    float initialWeight = synapse.getWeight();
    
    // Very different spike timing - should depress
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 20.0, 30.0}; // 10ms after pre
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to uncorrelated activity
    assert(finalWeight < initialWeight);
    
    std::cout << "    testHebbianDepression passed (Δ="
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianEmptySpikes() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};  // Fixed: replaced "entwich" with "2.0"
    
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
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void testHebbianSoftBounds() {
    // Phase 4: potentiation must shrink as the weight approaches its
    // bound, so repeated co-activity approaches 1.0 asymptotically
    // instead of clamping there in a few steps and erasing differences.
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Perfectly correlated spikes (strong potentiation)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    float firstDelta = synapse.getWeight() - 0.5f;
    if (!(firstDelta > 0.0f)) throw std::runtime_error("Hebbian soft-bound: no potentiation");
    
    float prevDelta = firstDelta;
    for (int i = 0; i < 200; ++i) {
        float before = synapse.getWeight();
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        float delta = synapse.getWeight() - before;
        if (!(delta >= 0.0f)) throw std::runtime_error("Hebbian soft-bound: negative increment");
        if (!(delta <= prevDelta + 1e-6f)) throw std::runtime_error("Hebbian soft-bound: increments did not shrink");
        prevDelta = delta;
    }
    // Approaches but never reaches the bound, even after 200 updates.
    if (!(synapse.getWeight() < 1.0f)) throw std::runtime_error("Hebbian soft-bound: weight reached bound");
    if (!(synapse.getWeight() > 0.5f)) throw std::runtime_error("Hebbian soft-bound: weight did not grow");
    
    std::cout << "    testHebbianSoftBounds passed (final weight="
              << synapse.getWeight() << ")" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianName();
    testHebbianPotentiation();
    testHebbianDepression();
    testHebbianEmptySpikes();
    testHebbianWeightBounds();
    testHebbianSoftBounds();
}

} // namespace test_hebbian
