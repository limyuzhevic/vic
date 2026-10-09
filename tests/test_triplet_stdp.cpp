// Triplet STDP Tests
// Comprehensive test suite for TripletSTDP implementation

#include "plasticity/STDP.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_triplet_stdp {

void testTripletCreation() {
    nlm::TripletSTDP triplet;
    
    assert(triplet.getName() != nullptr);
    assert(std::string(triplet.getName()) == "TripletSTDP");
    
    // Check default parameters
    assert(triplet.getTripletLTPWeight() > 0.0f);
    assert(triplet.getTripletLTDWeight() > 0.0f);
    assert(triplet.getTripletTimeConstant() > 0.0f);
    assert(triplet.getPairwiseLTPWeight() > 0.0f);
    assert(triplet.getPairwiseLTDWeight() > 0.0f);
    
    std::cout << "    testTripletCreation passed" << std::endl;
}

void testTripletConfiguration() {
    nlm::TripletSTDP triplet;
    
    // Configure triplet parameters
    triplet.setTripletLTPWeight(0.04f);
    triplet.setTripletLTDWeight(0.03f);
    triplet.setTripletTimeConstant(15.0f);
    triplet.setPairwiseLTPWeight(0.01f);
    triplet.setPairwiseLTDWeight(0.012f);
    
    assert(triplet.getTripletLTPWeight() == 0.04f);
    assert(triplet.getTripletLTDWeight() == 0.03f);
    assert(triplet.getTripletTimeConstant() == 15.0f);
    assert(triplet.getPairwiseLTPWeight() == 0.01f);
    assert(triplet.getPairwiseLTDWeight() == 0.012f);
    
    std::cout << "    testTripletConfiguration passed" << std::endl;
}

void testTripletValidation() {
    nlm::TripletSTDP triplet;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Triplet STDP requires at least 2 pre and 2 post spikes for triplet interactions
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // 3 pre spikes
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0}; // 3 post spikes
    
    // With this timing, we should see triplet effects
    // Pre-post-post pattern (0,5,15): strong potentiation
    // Pre-pre-post pattern (0,10,5): depression
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed due to triplet interactions
    assert(finalWeight != initialWeight);
    
    std::cout << "    testTripletValidation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testTripletTripletPotentiation() {
    nlm::TripletSTDP triplet;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create pre-post-post triplet (pre spikes at 0, post spikes at 5, 15)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-post-post triplet
    assert(finalWeight > initialWeight);
    
    std::cout << "    testTripletPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testTripletTripletDepression() {
    nlm::TripletSTDP triplet;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create pre-pre-post triplet (pre spikes at 0, 10, post spike at 5)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0};
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to pre-pre-post triplet
    assert(finalWeight < initialWeight);
    
    std::cout << "    testTripletDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testTripletWeightBounds() {
    nlm::TripletSTDP triplet;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    // Test minimum weight bound
    synapse.setWeight(-0.99f);
    
    // Strong depression
    std::vector<nlm::Timestamp> preSpikes2 = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes2 = {30.0, 40.0, 50.0};
    
    triplet.update(&synapse, preSpikes2, postSpikes2, 0.001);
    
    // Weight should be clamped to min
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testTripletWeightBounds passed" << std::endl;
}

void testTripletNormalization() {
    nlm::TripletSTDP triplet;
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // High firing rate (many spikes in short time)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    
    triplet.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float highRateWeight = synapse.getWeight();
    
    // Low firing rate (few spikes over longer time)
    synapse.setWeight(0.5f);
    std::vector<nlm::Timestamp> preSpikes2 = {0.0, 50.0, 100.0};
    std::vector<nlm::Timestamp> postSpikes2 = {10.0, 60.0, 110.0};
    
    triplet.update(&synapse, preSpikes2, postSpikes2, 0.001);
    
    float lowRateWeight = synapse.getWeight();
    
    // High firing rate should result in smaller weight changes due to normalization
    // (weight change should be closer to initial weight)
    assert(std::abs(highRateWeight - initialWeight) <= std::abs(lowRateWeight - initialWeight));
    
    std::cout << "    testTripletNormalization passed" << std::endl;
}

void testTripletParameters() {
    nlm::TripletSTDP triplet;
    
    // Test parameter clamping
    triplet.setTripletLTPWeight(-1.0f); // Should be clamped to 0.0
    assert(triplet.getTripletLTPWeight() >= 0.0f);
    
    triplet.setTripletLTDWeight(10.0f); // Should be clamped to 0.1
    assert(triplet.getTripletLTDWeight() <= 0.1f);
    
    triplet.setTripletTimeConstant(0.0f); // Should be clamped to 5.0
    assert(triplet.getTripletTimeConstant() >= 5.0f);
    
    triplet.setPairwiseLTPWeight(-0.5f); // Should be clamped to 0.0
    assert(triplet.getPairwiseLTPWeight() >= 0.0f);
    
    triplet.setPairwiseLTDWeight(2.0f); // Should be clamped to 0.1
    assert(triplet.getPairwiseLTDWeight() <= 0.1f);
    
    std::cout << "    testTripletParameters passed" << std::endl;
}

void runAll() {
    std::cout << "Running Triplet STDP tests..." << std::endl;
    testTripletCreation();
    testTripletConfiguration();
    testTripletValidation();
    testTripletTripletPotentiation();
    testTripletTripletDepression();
    testTripletWeightBounds();
    testTripletNormalization();
    testTripletParameters();
    std::cout << "All Triplet STDP tests passed!" << std::endl;
}

} // namespace test_triplet_stdp
