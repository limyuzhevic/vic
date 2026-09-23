// Triplet STDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/TripletSTDP.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_tripletstdp {

void testTripletSTDPCreation() {
    nlm::TripletSTDP rule;
    
    assert(rule.getLTPWeightA() == 0.01f);
    assert(rule.getLTDWeightA() == 0.012f);
    assert(rule.getLTPWeightB() == 0.003f);
    assert(rule.getLTDWeightB() == 0.003f);
    assert(rule.getTimeConstant() == 20.0f);
    
    std::cout << "    testTripletSTDPCreation passed" << std::endl;
}

void testTripletSTDPConfiguration() {
    nlm::TripletSTDP rule;
    
    rule.setLTPWeightA(0.02f);
    rule.setLTDWeightA(0.024f);
    rule.setLTPWeightB(0.006f);
    rule.setLTDWeightB(0.006f);
    rule.setTimeConstant(30.0f);
    
    assert(rule.getLTPWeightA() == 0.02f);
    assert(rule.getLTDWeightA() == 0.024f);
    assert(rule.getLTPWeightB() == 0.006f);
    assert(rule.getLTDWeightB() == 0.006f);
    assert(rule.getTimeConstant() == 30.0f);
    
    std::cout << "    testTripletSTDPConfiguration passed" << std::endl;
}

void testTripletSTDPName() {
    nlm::TripletSTDP rule;
    
    assert(rule.getName() != nullptr);
    assert(std::string(rule.getName()) == "TripletSTDP");
    
    std::cout << "    testTripletSTDPName passed" << std::endl;
}

void testTripletSTDPAdvancedDynamics() {
    nlm::TripletSTDP rule;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Simulate complex spike patterns with triplet interactions
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0, 10.0, 15.0};
    std::vector<nlm::Timestamp> postSpikes = {3.0, 8.0, 13.0, 18.0};
    
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed
    assert(std::abs(finalWeight - initialWeight) > 1e-5f);
    
    std::cout << "    testTripletSTDPAdvancedDynamics passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testTripletSTDPWeightBounds() {
    nlm::TripletSTDP rule;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    rule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to bounds
    assert(synapse.getWeight() <= 1.0f);
    assert(synapse.getWeight() >= -1.0f);  // min bound
    
    std::cout << "    testTripletSTDPWeightBounds passed" << std::endl;
}

void runAll() {
    std::cout << "Running Triplet STDP tests..." << std::endl;
    testTripletSTDPCreation();
    testTripletSTDPConfiguration();
    testTripletSTDPName();
    testTripletSTDPAdvancedDynamics();
    testTripletSTDPWeightBounds();
}

} // namespace test_tripletstdp