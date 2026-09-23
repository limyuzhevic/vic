// TripletSTDP Tests
#include "plasticity/TripletSTDP.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_triplet_stdp {

void testTripletSTDPCreation() {
    nlm::TripletSTDP tripletStdp;
    
    assert(tripletStdp.getA1() == 0.01f);
    assert(tripletStdp.getA2() == 0.01f);
    assert(tripletStdp.getA3() == 0.01f);
    assert(tripletStdp.getTau1() == 20.0f);
    assert(tripletStdp.getTau2() == 200.0f);
    assert(tripletStdp.getBaselineWeight() == 0.0f);
    
    std::cout << "    testTripletSTDPCreation passed" << std::endl;
}

void testTripletSTDPConfiguration() {
    nlm::TripletSTDP tripletStdp;
    
    tripletStdp.setA1(0.05f);
    tripletStdp.setA2(0.06f);
    tripletStdp.setA3(0.07f);
    tripletStdp.setTau1(25.0f);
    tripletStdp.setTau2(250.0f);
    tripletStdp.setBaselineWeight(0.1f);
    
    assert(tripletStdp.getA1() == 0.05f);
    assert(tripletStdp.getA2() == 0.06f);
    assert(tripletStdp.getA3() == 0.07f);
    assert(tripletStdp.getTau1() == 25.0f);
    assert(tripletStdp.getTau2() == 250.0f);
    assert(tripletStdp.getBaselineWeight() == 0.1f);
    
    std::cout << "    testTripletSTDPConfiguration passed" << std::endl;
}

void testTripletSTDPPotentiation() {
    nlm::TripletSTDP tripletStdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // ms (5-15ms after pre)
    
    tripletStdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    assert(finalWeight > initialWeight);
    
    std::cout << "    testTripletSTDPPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testTripletSTDPDepression() {
    nlm::TripletSTDP tripletStdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // ms (5-15ms before pre)
    
    tripletStdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    assert(finalWeight < initialWeight);
    
    std::cout << "    testTripletSTDPDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testTripletSTDPBaselineWeight() {
    nlm::TripletSTDP tripletStdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    
    // Set baseline weight
    float baseline = 0.2f;
    tripletStdp.setBaselineWeight(baseline);
    
    // The triplet STDP should respect baseline when updating weights
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0};
    
    tripletStdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be influenced by baseline
    assert(std::abs(synapse.getWeight() - baseline) < 0.5f);
    
    std::cout << "    testTripletSTDPBaselineWeight passed" << std::endl;
}

void runAll() {
    std::cout << "Running TripletSTDP tests..." << std::endl;
    testTripletSTDPCreation();
    testTripletSTDPConfiguration();
    testTripletSTDPPotentiation();
    testTripletSTDPDepression();
    testTripletSTDPBaselineWeight();
}

} // namespace test_triplet_stdp