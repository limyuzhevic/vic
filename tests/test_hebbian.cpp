// Hebbian Learning Rule Tests
// Tests correlation-based learning between neurons

#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    assert(hebbian.getMinWeight() == -1.0f);
    
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

void testHebbianBasicLearning() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre and post fire simultaneously (zero time difference)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // Different timing
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed based on correlation
    assert(finalWeight != initialWeight);
    
    std::cout << "    testHebbianBasicLearning passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianPositiveCorrelation() {
    nlm::Hebbian hebbian;
    hebbian.setLearningRate(0.02f);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Spike timing that should produce strong positive correlation
    // Pre fires just before post (within Hebbian window)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0, 40.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 25.0, 45.0};  // 5ms after pre
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // With positive correlation, weight should increase
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianPositiveCorrelation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianNegativeCorrelation() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Inhibitory);
    synapse.setWeight(-0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Spike timing that produces negative correlation (post before pre)
    std::vector<nlm::Timestamp> preSpikes = {10.0, 30.0, 50.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 25.0, 45.0};  // 5ms before pre
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed (magnitude may increase for inhibitory)
    assert(finalWeight != initialWeight);
    
    std::cout << "    testHebbianNegativeCorrelation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::Hebbian hebbian;
    
    // Create synapse at extreme weight
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong learning
    std::vector<nlm::Timestamp> preSpikes = {0.0, 0.1, 0.2, 0.3, 0.4};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 1.1, 1.2, 1.3, 1.4};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not exceed bounds
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void testHebbianNoSpikes() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Empty spike vectors
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes;
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not change
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testHebbianNoSpikes passed" << std::endl;
}

void testHebbianLearningRateBounds() {
    nlm::Hebbian hebbian;
    
    // Test learning rate clamping
    hebbian.setLearningRate(1.5f);  // Above max
    assert(hebbian.getLearningRate() == 1.0f);  // Should be clamped to 1.0
    
    hebbian.setLearningRate(-0.5f);  // Below min
    assert(hebbian.getLearningRate() == 0.0f);  // Should be clamped to 0.0
    
    std::cout << "    testHebbianLearningRateBounds passed" << std::endl;
}

void testHebbianIntegrationWithSynapse() {
    nlm::Hebbian hebbian;
    
    // Create synapse with plasticity flags
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.enablePlasticity(true, true, false);
    
    // Hebbian should only affect Hebbian plasticity
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should have changed
    assert(synapse.getWeight() != 0.5f);
    
    std::cout << "    testHebbianIntegrationWithSynapse passed" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianName();
    testHebbianBasicLearning();
    testHebbianPositiveCorrelation();
    testHebbianNegativeCorrelation();
    testHebbianWeightBounds();
    testHebbianNoSpikes();
    testHebbianLearningRateBounds();
    testHebbianIntegrationWithSynapse();
}

} // namespace test_hebbian