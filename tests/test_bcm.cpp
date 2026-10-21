// BCMRule Tests
// Bienenstock-Cooper-Munro learning rule for homeostatic stability

#include "plasticity/BCMRule.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_bcm {

void testBCMCreation() {
    nlm::BCMRule bcm;
    
    assert(bcm.getName() != nullptr);
    assert(std::string(bcm.getName()) == "BCMRule");
    
    // Check default parameters
    assert(bcm.getLearningRate() == 0.01f);
    assert(bcm.getThreshold() == 0.5f);
    assert(bcm.getThetaMinus() == 0.3f);
    assert(bcm.getThetaPlus() == 0.7f);
    assert(bcm.getMaxWeight() == 1.0f);
    assert(bcm.getMinWeight() == -1.0f);
    
    std::cout << "    testBCMCreation passed" << std::endl;
}

void testBCMConfiguration() {
    nlm::BCMRule bcm;
    
    bcm.setLearningRate(0.05f);
    bcm.setThreshold(0.6f);
    bcm.setThetaMinus(0.2f);
    bcm.setThetaPlus(0.9f);
    bcm.setMaxWeight(2.0f);
    bcm.setMinWeight(-2.0f);
    
    assert(bcm.getLearningRate() == 0.05f);
    assert(bcm.getThreshold() == 0.6f);
    assert(bcm.getThetaMinus() == 0.2f);
    assert(bcm.getThetaPlus() == 0.9f);
    assert(bcm.getMaxWeight() == 2.0f);
    assert(bcm.getMinWeight() == -2.0f);
    
    std::cout << "    testBCMConfiguration passed" << std::endl;
}

void testBCMLearningRule() {
    nlm::BCMRule bcm;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Simulate post-synaptic activity above threshold - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0};  // More post spikes
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased (postActivity > threshold)
    assert(finalWeight > initialWeight);
    
    std::cout << "    testBCMLearningRule potentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMDepression() {
    nlm::BCMRule bcm;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Simulate post-synaptic activity below threshold - should depress
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // Fewer post spikes
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased (postActivity < threshold)
    assert(finalWeight < initialWeight);
    
    std::cout << "    testBCMDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMSlidingThreshold() {
    nlm::BCMRule bcm;
    
    // Reset threshold to initial value
    bcm.resetThreshold();
    assert(bcm.getThreshold() == 0.3f);  // thetaMinus
    
    // Simulate high post-synaptic activity
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0, 40.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0, 45.0, 55.0};  // High activity
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Sliding threshold should have increased due to high activity
    float slidingThreshold = bcm.getSlidingThreshold();
    assert(slidingThreshold > 0.3f);
    
    std::cout << "    testBCMSlidingThreshold passed (threshold=" << slidingThreshold << ")" << std::endl;
}

void testBCMLowActivityDepression() {
    nlm::BCMRule bcm;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.8f);
    
    float initialWeight = synapse.getWeight();
    
    // Simulate very low post-synaptic activity
    std::vector<nlm::Timestamp> preSpikes = {0.0, 50.0, 100.0};  // Sparse pre
    std::vector<nlm::Timestamp> postSpikes = {5.0};  // Single post spike
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased significantly due to low activity
    assert(finalWeight < initialWeight);
    
    std::cout << "    testBCMLowActivityDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMLowActivityHomeostasis() {
    nlm::BCMRule bcm;
    
    // Reset to homeostatically stable state
    bcm.resetThreshold();
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Simulate activity patterns that approach homeostasis
    std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0, 40.0, 60.0, 80.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 30.0, 50.0, 70.0, 90.0, 110.0};
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float slidingThreshold = bcm.getSlidingThreshold();
    float synapseWeight = synapse.getWeight();
    
    // Should approach homeostatically stable state (weight close to threshold)
    assert(std::abs(synapseWeight - slidingThreshold) < 0.2f);
    
    std::cout << "    testBCMLowActivityHomeostasis passed (weight=" 
              << synapseWeight << ", threshold=" << slidingThreshold << ")" << std::endl;
}

void testBCMLargeWeightChange() {
    nlm::BCMRule bcm;
    
    // Create synapse with room for large changes
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.0f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0};
    std::vector<nlm::Timestamp> postSpikes = {2.0, 7.0, 12.0, 17.0, 22.0, 27.0, 32.0, 37.0, 42.0, 47.0, 52.0, 57.0};
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed significantly
    assert(std::abs(finalWeight) > 0.01f);
    
    // Weight should be within bounds
    assert(finalWeight >= -2.0f);
    assert(finalWeight <= 2.0f);
    
    std::cout << "    testBCMLargeWeightChange passed (weight=" << finalWeight << ")" << std::endl;
}

void testBCMWeightClamping() {
    nlm::BCMRule bcm;
    
    bcm.setMaxWeight(0.6f);
    bcm.setMinWeight(-0.6f);
    
    nlm::Synapse synapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.9f);  // Above max
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 0.6f);
    
    // Test lower bound
    nlm::Synapse synapse2(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(-0.9f);  // Below min
    
    bcm.update(&synapse2, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to min
    assert(synapse2.getWeight() >= -0.6f);
    
    std::cout << "    testBCMWeightClamping passed" << std::endl;
}

void testBCMLimitParameterClamping() {
    nlm::BCMRule bcm;
    
    // Test clamping of parameter ranges
    bcm.setLearningRate(-0.01f);  // Should clamp to min
    assert(bcm.getLearningRate() == 0.0f);
    
    bcm.setLearningRate(0.2f);   // Should clamp to max
    assert(bcm.getLearningRate() == 0.1f);
    
    bcm.setThreshold(-0.1f);     // Should clamp to min
    assert(bcm.getThreshold() == 0.0f);
    
    bcm.setThreshold(3.0f);      // Should clamp to max
    assert(bcm.getThreshold() == 2.0f);
    
    bcm.setThetaMinus(-0.1f);    // Should clamp to min
    assert(bcm.getThetaMinus() == 0.0f);
    
    bcm.setThetaMinus(3.0f);     // Should clamp to max
    assert(bcm.getThetaMinus() == 2.0f);
    
    bcm.setThetaPlus(-0.1f);     // Should clamp to min
    assert(bcm.getThetaPlus() == 0.0f);
    
    bcm.setThetaPlus(3.0f);      // Should clamp to max
    assert(bcm.getThetaPlus() == 2.0f);
    
    std::cout << "    testBCMLimitParameterClamping passed" << std::endl;
}

void runAll() {
    std::cout << "Running BCMRule tests..." << std::endl;
    testBCMCreation();
    testBCMConfiguration();
    testBCMLearningRule();
    testBCMDepression();
    testBCMSlidingThreshold();
    testBCMLowActivityDepression();
    testBCMLowActivityHomeostasis();
    testBCMLargeWeightChange();
    testBCMWeightClamping();
    testBCMLimitParameterClamping();
}

} // namespace test_bcm
