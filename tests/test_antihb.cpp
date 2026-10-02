// AntiHebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/AntiHebbian.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_antihb {

void testAntiHebbianCreation() {
    nlm::AntiHebbianRule antiHebbian;
    
    assert(antiHebbian.getDecayRate() == 0.001f);
    assert(antiHebbian.getTargetWeight() == 0.0f);
    assert(antiHebbian.getLearningRate() == 0.01f);
    
    std::cout << "    testAntiHebbianCreation passed" << std::endl;
}

void testAntiHebbianConfiguration() {
    nlm::AntiHebbianRule antiHebbian;
    
    antiHebbian.setDecayRate(0.005f);
    antiHebbian.setTargetWeight(0.1f);
    
    assert(antiHebbian.getDecayRate() == 0.005f);
    assert(antiHebbian.getTargetWeight() == 0.1f);
    
    std::cout << "    testAntiHebbianConfiguration passed" << std::endl;
}

void testAntiHebbianName() {
    nlm::AntiHebbianRule antiHebbian;
    
    assert(antiHebbian.getName() != nullptr);
    assert(std::string(antiHebbian.getName()) == "AntiHebbian");
    
    std::cout << "    testAntiHebbianName passed" << std::endl;
}

void testAntiHebbianHomeostaticDecay() {
    nlm::AntiHebbianRule antiHebbian;
    antiHebbian.setDecayRate(0.01f);
    antiHebbian.setTargetWeight(0.5f);
    
    // Create a synapse with weight above target
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.8f);  // Above target
    
    float initialWeight = synapse.getWeight();
    
    // Spike activity should cause homeostatic decay toward target
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    antiHebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should move toward target (0.5) from 0.8
    // AntiHebbian should decrease weight when above target
    assert(finalWeight < initialWeight);
    assert(finalWeight > 0.5f);  // Should approach but not exceed target
    
    std::cout << "    testAntiHebbianHomeostaticDecay passed (" 
              << initialWeight << " -> " << finalWeight << ")" << std::endl;
}

void testAntiHebbianUnderHomeostatic() {
    nlm::AntiHebbianRule antiHebbian;
    antiHebbian.setDecayRate(0.01f);
    antiHebbian.setTargetWeight(0.5f);
    
    // Create a synapse with weight below target
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.2f);  // Below target
    
    float initialWeight = synapse.getWeight();
    
    // Spike activity
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0};
    
    antiHebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight may increase slightly but should remain below target
    assert(finalWeight >= initialWeight);
    assert(finalWeight <= 0.5f);
    
    std::cout << "    testAntiHebbianUnderHomeostatic passed (" 
              << initialWeight << " -> " << finalWeight << ")" << std::endl;
}

void testAntiHebbianWeightBounds() {
    nlm::AntiHebbianRule antiHebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0};
    
    antiHebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testAntiHebbianWeightBounds passed" << std::endl;
}

void testAntiHebbianNullSynapse() {
    nlm::AntiHebbianRule antiHebbian;
    
    // Should not crash when synapse is null
    antiHebbian.update(nullptr, {}, {}, 0.001);
    antiHebbian.applyWeightChange(nullptr, 0.1f);
    
    std::cout << "    testAntiHebbianNullSynapse passed" << std::endl;
}

void runAll() {
    std::cout << "Running AntiHebbian tests..." << std::endl;
    testAntiHebbianCreation();
    testAntiHebbianConfiguration();
    testAntiHebbianName();
    testAntiHebbianHomeostaticDecay();
    testAntiHebbianUnderHomeostatic();
    testAntiHebbianWeightBounds();
    testAntiHebbianNullSynapse();
}

} // namespace test_antihb