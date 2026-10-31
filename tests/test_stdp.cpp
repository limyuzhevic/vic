// STDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <stdexcept>

namespace test_stdp {

void testSTDPCreation() {
    nlm::STDP stdp;
    
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    std::cout << "    testSTDPCreation passed" << std::endl;
}

void testSTDPConfiguration() {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstant() == 30.0f);
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testSTDPConfigure() {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    assert(stdp.getLTPWeight() == 0.03f);
    assert(stdp.getLTDWeight() == 0.035f);
    assert(stdp.getTimeConstant() == 25.0f);
    
    std::cout << "    testSTDPConfigure passed" << std::endl;
}

void testSTDPName() {
    nlm::STDP stdp;
    
    assert(stdp.getName() != nullptr);
    assert(std::string(stdp.getName()) == "STDP");
    
    std::cout << "    testSTDPName passed" << std::endl;
}

void testSTDPPotentiation() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    assert(finalWeight > initialWeight);
    
    std::cout << "    testSTDPPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPDepression() {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    assert(finalWeight < initialWeight);
    
    std::cout << "    testSTDPDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPEmptySpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
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

void testSTDPWeightBounds() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

void testSTDPSoftBounds();

void runAll() {
    std::cout << "Running STDP tests..." << std::endl;
    testSTDPCreation();
    testSTDPConfiguration();
    testSTDPConfigure();
    testSTDPName();
    testSTDPPotentiation();
    testSTDPDepression();
    testSTDPEmptySpikes();
    testSTDPWeightBounds();
    testSTDPSoftBounds();
}

void testSTDPSoftBounds() {
    // Phase 4: potentiation must shrink as the weight approaches its
    // bound, so repeated co-activity approaches 1.0 asymptotically
    // instead of clamping there in a few steps and erasing differences.
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);

    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);

    // Pre 5ms before post, in seconds (Brain steps at 1ms).
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {0.005};

    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float firstDelta = synapse.getWeight() - 0.5f;
    if (!(firstDelta > 0.0f)) throw std::runtime_error("STDP soft-bound: no potentiation");
    // Unscaled delta would be 0.01 * exp(-5/20) ~= 0.0078; soft scaling
    // by (1 - 0.5) must make the actual step smaller than that.
    if (!(firstDelta < 0.0078f)) throw std::runtime_error("STDP soft-bound: step not scaled down");

    float prevDelta = firstDelta;
    for (int i = 0; i < 200; ++i) {
        float before = synapse.getWeight();
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        float delta = synapse.getWeight() - before;
        if (!(delta >= 0.0f)) throw std::runtime_error("STDP soft-bound: negative increment");
        if (!(delta <= prevDelta + 1e-6f)) throw std::runtime_error("STDP soft-bound: increments did not shrink");
        prevDelta = delta;
    }
    // Approaches but never reaches the bound, even after 200 updates.
    if (!(synapse.getWeight() < 1.0f)) throw std::runtime_error("STDP soft-bound: weight reached bound");
    if (!(synapse.getWeight() > 0.5f)) throw std::runtime_error("STDP soft-bound: weight did not grow");

    std::cout << "    testSTDPSoftBounds passed (final weight="
              << synapse.getWeight() << ")" << std::endl;
}

} // namespace test_stdp
