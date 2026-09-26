// TripletSTDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/TripletSTDP.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_triplet_stdp {

void testTripletSTDPCreation() {
    nlm::TripletSTDP triplet;
    
    // Default parameters
    assert(triplet.getPairLTPWeight() == 0.01f);
    assert(triplet.getPairLTDWeight() == 0.012f);
    assert(triplet.getTripletLTPWeight() == 0.005f);
    assert(triplet.getTripletLTDWeight() == 0.006f);
    assert(triplet.getTripletTimeConstant() == 50.0f);
    assert(triplet.getMinWeight() == -1.0f);
    assert(triplet.getMaxWeight() == 1.0f);
    
    std::cout << "    testTripletSTDPCreation passed" << std::endl;
}

void testTripletSTDPConfiguration() {
    nlm::TripletSTDP triplet;
    
    triplet.setPairLTPWeight(0.05f);
    triplet.setPairLTDWeight(0.06f);
    triplet.setTripletLTPWeight(0.03f);
    triplet.setTripletLTDWeight(0.04f);
    triplet.setTripletTimeConstant(75.0f);
    triplet.setMinWeight(-0.5f);
    triplet.setMaxWeight(0.5f);
    
    assert(triplet.getPairLTPWeight() == 0.05f);
    assert(triplet.getPairLTDWeight() == 0.06f);
    assert(triplet.getTripletLTPWeight() == 0.03f);
    assert(triplet.getTripletLTDWeight() == 0.04f);
    assert(triplet.getTripletTimeConstant() == 75.0f);
    assert(triplet.getMinWeight() == -0.5f);
    assert(triplet.getMaxWeight() == 0.5f);
    
    std::cout << "    testTripletSTDPConfiguration passed" << std::endl;
}

void testTripletSTDPConfigure() {
    nlm::TripletSTDP triplet;
    
    triplet.configure(0.03f, 0.035f, 0.015f, 0.016f, 60.0f);
    
    assert(triplet.getPairLTPWeight() == 0.03f);
    assert(triplet.getPairLTDWeight() == 0.035f);
    assert(triplet.getTripletLTPWeight() == 0.015f);
    assert(triplet.getTripletLTDWeight() == 0.016f);
    assert(triplet.getTripletTimeConstant() == 60.0f);
    
    std::cout << "    testTripletSTDPConfigure passed" << std::endl;
}

void testTripletSTDPName() {
    nlm::TripletSTDP triplet;
    
    assert(triplet.getName() != nullptr);
    assert(std::string(triplet.getName()) == "TripletSTDP");
    
    std::cout << "    testTripletSTDPName passed" << std::endl;
}

void testTripletSTDPWeightBounds() {
    nlm::TripletSTDP triplet;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong triplet interactions
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    // Test minimum weight with depression
    synapse.setWeight(-0.99f);  // Near min
    preSpikes = {20.0, 21.0, 22.0, 23.0, 24.0};
    postSpikes = {10.0, 11.0, 12.0, 13.0, 14.0};  // Post before pre
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to min
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testTripletSTDPWeightBounds passed" << std::endl;
}

void testTripletSTDPTripletInteractions() {
    nlm::TripletSTDP triplet;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create a pre-post-pre triplet pattern (should potentiate)
    // Pre: 0, Post: 5, Pre: 10
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float tripletWeight = synapse.getWeight();
    
    // Reset and test with simple pair (less potentiation)
    synapse.setWeight(initialWeight);
    synapse.clearHistory();
    
    // Simple pre-post pattern (less potentiation than triplet)
    std::vector<nlm::Timestamp> preSpikesPair = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikesPair = {5.0};
    
    triplet.update(&synapse, preSpikesPair, postSpikesPair, 0.001);
    
    float pairWeight = synapse.getWeight();
    
    // Triplet pattern should produce more potentiation
    // (not strictly guaranteed due to exponential nature, but generally true)
    assert(tripletWeight >= pairWeight);
    
    std::cout << "    testTripletSTDPTripletInteractions passed (Δtriplet=" 
              << (tripletWeight - initialWeight) << ", Δpair=" 
              << (pairWeight - initialWeight) << ")" << std::endl;
}

void testTripletSTDPParameterChanges() {
    nlm::TripletSTDP triplet;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test with low triplet weights (should have less effect)
    triplet.setTripletLTPWeight(0.001f);
    triplet.setTripletLTDWeight(0.001f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightWithLowTriplet = synapse.getWeight();
    
    // Reset and test with high triplet weights
    synapse.setWeight(0.5f);
    synapse.clearHistory();
    
    triplet.setTripletLTPWeight(0.1f);
    triplet.setTripletLTDWeight(0.1f);
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightWithHighTriplet = synapse.getWeight();
    
    // Weight change should be proportional to triplet weights
    assert(weightWithHighTriplet > weightWithLowTriplet);
    
    std::cout << "    testTripletSTDPParameterChanges passed (low triplet Δ=" 
              << (weightWithLowTriplet - 0.5f) << ", high triplet Δ=" 
              << (weightWithHighTriplet - 0.5f) << ")" << std::endl;
}

void runAll() {
    std::cout << "Running TripletSTDP tests..." << std::endl;
    testTripletSTDPCreation();
    testTripletSTDPConfiguration();
    testTripletSTDPConfigure();
    testTripletSTDPName();
    testTripletSTDPWeightBounds();
    testTripletSTDPTripletInteractions();
    testTripletSTDPParameterChanges();
}

} // namespace test_triplet_stdp
