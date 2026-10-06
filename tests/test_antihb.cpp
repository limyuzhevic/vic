// AntiHebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/AntiHebbian.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_antihb {

void testAntiHebbianCreation() {
    nlm::AntiHebbianRule antihebbian;
    
    assert(antihebbian.getLearningRate() == 0.005f);
    assert(antihebbian.getMaxWeight() == 1.0f);
    assert(antihebbian.getMinWeight() == -1.0f);
    assert(antihebbian.getTimeWindow() == 20.0f);
    
    std::cout << "    testAntiHebbianCreation passed" << std::endl;
}

void testAntiHebbianConfiguration() {
    nlm::AntiHebbianRule antihebbian;
    
    antihebbian.setLearningRate(0.02f);
    antihebbian.setMaxWeight(0.9f);
    antihebbian.setMinWeight(-0.9f);
    antihebbian.setTimeWindow(30.0f);
    
    assert(antihebbian.getLearningRate() == 0.02f);
    assert(antihebbian.getMaxWeight() == 0.9f);
    assert(antihebbian.getMinWeight() == -0.9f);
    assert(antihebbian.getTimeWindow() == 30.0f);
    
    std::cout << "    testAntiHebbianConfiguration passed" << std::endl;
}

void testAntiHebbianConfigure() {
    nlm::AntiHebbianRule antihebbian;
    
    antihebbian.configure(0.01f, 0.8f, -0.8f, 25.0f);
    
    assert(antihebbian.getLearningRate() == 0.01f);
    assert(antihebbian.getMaxWeight() == 0.8f);
    assert(antihebbian.getMinWeight() == -0.8f);
    assert(antihebbian.getTimeWindow() == 25.0f);
    
    std::cout << "    testAntiHebbianConfigure passed" << std::endl;
}

void testAntiHebbianName() {
    nlm::AntiHebbianRule antihebbian;
    
    assert(antihebbian.getName() != nullptr);
    assert(std::string(antihebbian.getName()) == "AntiHebbian");
    
    std::cout << "    testAntiHebbianName passed" << std::endl;
}

void testAntiHebbianDepression() {
    nlm::AntiHebbianRule antihebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Both pre and post fire - should depress (anti-Hebbian)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    antihebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to anti-Hebbian rule
    assert(finalWeight <= initialWeight);
    
    std::cout << "    testAntiHebbianDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testAntiHebbianAntidepression() {
    nlm::AntiHebbianRule antihebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Only pre fires - should not depress (no correlated activity)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {};
    
    antihebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should not decrease (no correlated activity)
    assert(finalWeight == initialWeight);
    
    std::cout << "    testAntiHebbianAntidepression passed" << std::endl;
}

void testAntiHebbianEmptySpikes() {
    nlm::AntiHebbianRule antihebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    antihebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    antihebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testAntiHebbianEmptySpikes passed" << std::endl;
}

void testAntiHebbianWeightBounds() {
    nlm::AntiHebbianRule antihebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(-0.99f);  // Near min
    
    // Strong depression
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    antihebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to min
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testAntiHebbianWeightBounds passed" << std::endl;
}

void runAll() {
    std::cout << "Running AntiHebbian tests..." << std::endl;
    testAntiHebbianCreation();
    testAntiHebbianConfiguration();
    testAntiHebbianConfigure();
    testAntiHebbianName();
    testAntiHebbianDepression();
    testAntiHebbianAntidepression();
    testAntiHebbianEmptySpikes();
    testAntiHebbianWeightBounds();
}

} // namespace test_antihb
