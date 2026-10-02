// Hebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::HebbianRule hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    assert(hebbian.getMaxWeight() == 1.0f);
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::HebbianRule hebbian;
    
    hebbian.setLearningRate(0.02f);
    hebbian.setMaxWeight(2.0f);
    
    assert(hebbian.getLearningRate() == 0.02f);
    assert(hebbian.getMaxWeight() == 2.0f);
    
    std::cout << "    testHebbianConfiguration passed" << std::endl;
}

void testHebbianName() {
    nlm::HebbianRule hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "    testHebbianName passed" << std::endl;
}

void testHebbianBasicLearning() {
    nlm::HebbianRule hebbian;
    hebbian.setLearningRate(0.01f);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Spike coincidence should cause potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // 5ms overlap
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to correlated spikes
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianBasicLearning passed (" 
              << initialWeight << " -> " << finalWeight << ")" << std::endl;
}

void testHebbianStrongCoincidence() {
    nlm::HebbianRule hebbian;
    hebbian.setLearningRate(0.05f);  // Higher learning rate
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Strong coincidence (more overlapping spikes)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should increase more with stronger coincidence
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianStrongCoincidence passed (" 
              << initialWeight << " -> " << finalWeight << ")" << std::endl;
}

void testHebbianWeakCoincidence() {
    nlm::HebbianRule hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Weak coincidence (minimal overlap)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {100.0};  // Far apart
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Small change with weak coincidence
    assert(std::abs(finalWeight - initialWeight) < 0.01f);
    
    std::cout << "    testHebbianWeakCoincidence passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianLearningBounds() {
    nlm::HebbianRule hebbian;
    hebbian.setMaxWeight(1.5f);
    hebbian.setLearningRate(0.1f);  // High learning rate
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.9f);  // Near max
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.5f);
    
    std::cout << "    testHebbianLearningBounds passed" << std::endl;
}

void testHebbianNullSynapse() {
    nlm::HebbianRule hebbian;
    
    // Should not crash when synapse is null
    hebbian.update(nullptr, {}, {}, 0.001);
    hebbian.applyWeightChange(nullptr, 0.1f);
    
    std::cout << "    testHebbianNullSynapse passed" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::HebbianRule hebbian;
    hebbian.setMaxWeight(2.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Try to apply large negative weight change
    hebbian.applyWeightChange(&synapse, -1.0f);  // Should clamp to 0.0f
    
    // Weight should not go below 0
    assert(synapse.getWeight() >= 0.0f);
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void testHebbianNegativeLearning() {
    nlm::HebbianRule hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // No coincidence should result in minimal learning
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {100.0};  // No overlap
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // With no coincidence, weight change should be minimal
    assert(std::abs(finalWeight - initialWeight) < 0.01f);
    
    std::cout << "    testHebbianNegativeLearning passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianParameters() {
    nlm::HebbianRule hebbian;
    
    // Test all parameters
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    
    // Modify parameters
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 2.0f);
    
    std::cout << "    testHebbianParameters passed" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianName();
    testHebbianBasicLearning();
    testHebbianStrongCoincidence();
    testHebbianWeakCoincidence();
    testHebbianLearningBounds();
    testHebbianNullSynapse();
    testHebbianWeightBounds();
    testHebbianNegativeLearning();
    testHebbianParameters();
}

} // namespace test_hebbian