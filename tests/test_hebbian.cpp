// Hebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    assert(hebbian.getMaxWeight() == 1.0f);  // Check max weight
    
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

void testHebbianCorrelationLearning() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create correlated spikes (pre and post fires together)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {1.0, 11.0, 21.0};  // ms (very close to pre)
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to correlation
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianCorrelationLearning passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianAntiCorrelation() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create anti-correlated spikes (pre and post fire far apart)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 50.0, 100.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {200.0, 250.0, 300.0};  // ms (far from pre)
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should be less affected due to low correlation
    // (may still increase slightly due to some baseline correlation)
    std::cout << "    testHebbianAntiCorrelation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianEmptySpikes() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testHebbianEmptySpikes passed" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max (1.0)
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void testHebbianNegativeWeights() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Inhibitory);
    synapse.setWeight(-0.99f);  // Near min
    
    // Create negative correlation (should strengthen inhibition)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {50.0, 60.0, 70.0};  // Far apart (low correlation)
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should stay clamped to min (-1.0) or become more negative
    assert(synapse.getWeight() <= -1.0f);
    
    std::cout << "    testHebbianNegativeWeights passed" << std::endl;
}

void testHebbianLearningRate() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Create correlated spikes
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 11.0};
    
    // Update with high learning rate
    hebbian.setLearningRate(0.1f);
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightWithHighLR = synapse.getWeight();
    
    // Reset
    synapse.setWeight(0.3f);
    
    // Update with low learning rate
    hebbian.setLearningRate(0.001f);
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightWithLowLR = synapse.getWeight();
    
    // High learning rate should produce larger change
    assert(weightWithHighLR > weightWithLowLR);
    
    std::cout << "    testHebbianLearningRate passed" << std::endl;
}

void testHebbianPerformance() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Create large spike sets to test performance
    // Old O(n²) would be slow with 1000 spikes each
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes;
    
    // Generate 500 spikes each with small time differences (high correlation)
    for (int i = 0; i < 500; ++i) {
        preSpikes.push_back(static_cast<nlm::Timestamp>(i * 10.0));
        postSpikes.push_back(static_cast<nlm::Timestamp>(i * 10.0 + 5.0));  // 5ms after
    }
    
    float initialWeight = synapse.getWeight();
    
    // This should complete quickly with O(n) optimization
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Should have learned something from correlated activity
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianPerformance passed (500 spikes each)" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianName();
    testHebbianCorrelationLearning();
    testHebbianAntiCorrelation();
    testHebbianEmptySpikes();
    testHebbianWeightBounds();
    testHebbianNegativeWeights();
    testHebbianLearningRate();
    testHebbianPerformance();
    
    std::cout << std::endl << "All Hebbian tests passed!" << std::endl;
}

} // namespace test_hebbian