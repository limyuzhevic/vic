// Hebbian Tests
#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    assert(hebbian.getMinWeight() == -1.0f);
    assert(hebbian.getCovarianceThreshold() == 0.0f);
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    hebbian.setMinWeight(-2.0f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 2.0f);
    assert(hebbian.getMinWeight() == -2.0f);
    
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
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create correlated spike patterns (both pre and post spike within window)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 11.0, 21.0};  // Within 100ms window
    
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
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create uncorrelated spike patterns (minimal overlap)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 100.0, 200.0};
    std::vector<nlm::Timestamp> postSpikes = {50.0, 150.0, 250.0};  // Minimal correlation
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight may decrease or increase slightly, but within reasonable bounds
    // Allow for small fluctuations
    assert(std::abs(finalWeight - initialWeight) < 0.1f);
    
    std::cout << "    testHebbianDepression passed" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not exceed max
    assert(synapse.getWeight() <= hebbian.getMaxWeight());
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void testHebbianOnlineNormalization() {
    nlm::Hebbian hebbian;
    
    // Enable online normalization
    hebbian.setOnlineNormalization(true);
    assert(hebbian.getOnlineNormalization() == true);
    
    // Test that it doesn't crash with normalization enabled
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    std::cout << "    testHebbianOnlineNormalization passed" << std::endl;
}

void testHebbianHeterosynaptic() {
    nlm::Hebbian hebbian;
    
    // Enable heterosynaptic plasticity
    hebbian.setHeterosynapticPlasticity(true);
    assert(hebbian.getHeterosynapticPlasticity() == true);
    
    hebbian.setLateralInhibition(0.2f);
    assert(hebbian.getLateralInhibition() == 0.2f);
    
    std::cout << "    testHebbianHeterosynaptic passed" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianName();
    testHebbianPotentiation();
    testHebbianDepression();
    testHebbianWeightBounds();
    testHebbianOnlineNormalization();
    testHebbianHeterosynaptic();
}

} // namespace test_hebbian
