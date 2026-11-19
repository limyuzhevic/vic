// Hebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
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

void testHebbianExcitatoryPotentiation() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create correlated pre/post spikes (fire together)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 0.01, 0.02};  // ms
    std::vector<nlm::Timestamp> postSpikes = {0.005, 0.015, 0.025};  // ms (5ms after pre)
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to coactivity
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianExcitatoryPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianInhibitoryDepression() {
    nlm::Hebbian hebbian;
    
    // Create an inhibitory synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Inhibitory);
    synapse.setWeight(-0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Create correlated pre/post spikes
    std::vector<nlm::Timestamp> preSpikes = {0.0, 0.01, 0.02};
    std::vector<nlm::Timestamp> postSpikes = {0.005, 0.015, 0.025};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have become more negative (strengthened inhibition)
    assert(finalWeight < initialWeight);
    
    std::cout << "    testHebbianInhibitoryDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianUncorrelatedSpikes() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Uncorrelated spikes (far apart)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 0.1};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 1.1};  // 1s later
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testHebbianUncorrelatedSpikes passed" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.9f);  // Near max
    
    // Create correlated spikes to try to push weight over max
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {0.5, 1.5, 2.5, 3.5, 4.5};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void testHebbianSoftBounds() {
    // Phase 4: Hebbian strengthening must shrink as weight approaches bound,
    // so repeated coactivity approaches max asymptotically instead of clamping.
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Pre 5ms before post, in seconds (Brain steps at 1ms).
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {0.005};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    float firstDelta = synapse.getWeight() - 0.5f;
    if (!(firstDelta > 0.0f)) throw std::runtime_error("Hebbian soft-bound: no potentiation");
    // Unscaled delta would be 0.01; soft scaling by (1 - 0.5) must make
    // the actual step smaller.
    if (!(firstDelta < 0.005f)) throw std::runtime_error("Hebbian soft-bound: step not scaled down");
    
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
    testHebbianExcitatoryPotentiation();
    testHebbianInhibitoryDepression();
    testHebbianUncorrelatedSpikes();
    testHebbianWeightBounds();
    testHebbianSoftBounds();
}

} // namespace test_hebbian