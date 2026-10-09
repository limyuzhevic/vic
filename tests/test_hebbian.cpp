// Hebbian Tests
// Phase 2: Real Neural Computation Tests

#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    assert(hebbian.getBaselineActivity() == 0.01f);
    assert(hebbian.getLearningRatePotentiation() == 0.01f);
    assert(hebbian.getLearningRateDepression() == 0.012f);
    assert(hebbian.getCorrelationWindow() == 100.0f);
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::Hebbian hebbian;
    
    hebbian.configure(0.05f, 0.02f, 0.05f, 0.06f, 0.8f, 0.2f, 200.0f, true, true, 0.02f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getBaselineActivity() == 0.02f);
    assert(hebbian.getLearningRatePotentiation() == 0.05f);
    assert(hebbian.getLearningRateDepression() == 0.06f);
    assert(hebbian.getMaxWeight() == 0.8f);
    assert(hebbian.getMaxWeightChange() == 0.2f);
    assert(hebbian.getCorrelationWindow() == 200.0f);
    assert(hebbian.getAdaptiveWindow() == true);
    assert(hebbian.getUseOnlineStatistics() == true);
    assert(hebbian.getStatisticsDecay() == 0.02f);
    
    std::cout << "    testHebbianConfiguration passed" << std::endl;
}

void testHebbianVariantSelection() {
    nlm::Hebbian hebbian;
    
    hebbian.setVariant(nlm::Hebbian::Variant::Oja);
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "OjaHebbian");
    
    hebbian.setVariant(nlm::Hebbian::Variant::BCM);
    assert(std::string(hebbian.getName()) == "BCMHebbian");
    
    hebbian.setVariant(nlm::Hebbian::Variant::HebbianCovariance);
    assert(std::string(hebbian.getName()) == "HebbianCovariance");
    
    std::cout << "    testHebbianVariantSelection passed" << std::endl;
}

void testHebbianBasicLearning() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre and post fire together (positive correlation)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (5ms offset)
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to positive correlation
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianBasicLearning passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianCovarianceLearning() {
    nlm::HebbianCovariance hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive correlation)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {1.0, 11.0, 21.0};  // ms (1ms offset)
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to covariance
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianCovarianceLearning passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianOjaLearning() {
    nlm::OjaHebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre and post fire (Oja normalization)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {1.0, 11.0};  // ms
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should be normalized
    assert(std::abs(finalWeight) <= 1.0f);
    
    std::cout << "    testHebbianOjaLearning passed (w=" 
              << finalWeight << ")" << std::endl;
}

void testHebbianBCMLearning() {
    nlm::BCMHebbian hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Set BCM parameters
    hebbian.setBCMTheta(0.1f);
    hebbian.setBCMLearningRate(0.02f);
    
    // Post fires above threshold (BCM should potentiate)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {1.0, 11.0};  // ms
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should be within bounds
    assert(std::abs(finalWeight) <= 1.0f);
    
    std::cout << "    testHebbianBCMLearning passed (w=" 
              << finalWeight << ")" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    // Test negative weight clamping
    synapse.setWeight(-0.99f);  // Near min
    std::vector<nlm::Timestamp> preSpikes2 = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes2 = {5.0, 15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes2, postSpikes2, 0.001);
    
    // Weight should be clamped to min
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void testHebbianEmptySpikes() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
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

void testHebbianStatistics() {
    nlm::Hebbian hebbian;
    
    // Test online statistics
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 11.0, 21.0};
    
    hebbian.updateStatistics(preSpikes, postSpikes, 0.001);
    
    assert(hebbian.getPreSpikeMean() >= 0.0f);
    assert(hebbian.getPostSpikeMean() >= 0.0f);
    
    hebbian.resetOnlineStatistics();
    assert(hebbian.getPreSpikeMean() == 0.0f);
    assert(hebbian.getPostSpikeMean() == 0.0f);
    
    std::cout << "    testHebbianStatistics passed" << std::endl;
}

void testHebbianBatchProcessing() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Create batch data
    std::vector<std::vector<nlm::Timestamp>> preSpikeBatches;
    std::vector<std::vector<nlm::Timestamp>> postSpikeBatches;
    std::vector<nlm::TimestepDuration> dts;
    
    // Batch 1
    preSpikeBatches.push_back(std::vector<nlm::Timestamp>{0.0, 10.0});
    postSpikeBatches.push_back(std::vector<nlm::Timestamp>{1.0, 11.0});
    dts.push_back(0.001);
    
    // Batch 2
    preSpikeBatches.push_back(std::vector<nlm::Timestamp>{20.0, 30.0});
    postSpikeBatches.push_back(std::vector<nlm::Timestamp>{21.0, 31.0});
    dts.push_back(0.001);
    
    hebbian.batchUpdate(&synapse, preSpikeBatches, postSpikeBatches, dts);
    
    // Weight should have changed due to batch processing
    assert(std::abs(synapse.getWeight() - 0.5f) > 1e-6f);
    
    std::cout << "    testHebbianBatchProcessing passed (w=" 
              << synapse.getWeight() << ")" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianVariantSelection();
    testHebbianBasicLearning();
    testHebbianCovarianceLearning();
    testHebbianOjaLearning();
    testHebbianBCMLearning();
    testHebbianWeightBounds();
    testHebbianEmptySpikes();
    testHebbianStatistics();
    testHebbianBatchProcessing();
}

} // namespace test_hebbian