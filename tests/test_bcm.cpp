// BCM Tests
#include "brain/Synapse.hpp"
#include "plasticity/BCMRule.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_bcm {

void testBCMCreation() {
    nlm::BCMRule bcm;
    
    assert(bcm.getTheta() == 0.2f);
    assert(bcm.getBCMLearningRate() == 0.01f);
    assert(bcm.getBCMTimeConstant() == 100.0f);
    
    std::cout << "    testBCMCreation passed" << std::endl;
}

void testBCMConfiguration() {
    nlm::BCMRule bcm;
    
    bcm.setTheta(0.15f);
    bcm.setBCMLearningRate(0.02f);
    bcm.setBCMTimeConstant(80.0f);
    
    assert(bcm.getTheta() == 0.15f);
    assert(bcm.getBCMLearningRate() == 0.02f);
    assert(bcm.getBCMTimeConstant() == 80.0f);
    
    std::cout << "    testBCMConfiguration passed" << std::endl;
}

void testBCMMultipleConfigure() {
    nlm::BCMRule bcm;
    
    bcm.configure(0.1f, 0.12f, 90.0f);
    
    assert(bcm.getTheta() == 0.1f);
    assert(bcm.getBCMLearningRate() == 0.1f);
    assert(bcm.getBCMTimeConstant() == 90.0f);
    
    std::cout << "    testBCMMultipleConfigure passed" << std::endl;
}

void testBCMBasicName() {
    nlm::BCMRule bcm;
    
    assert(bcm.getName() != nullptr);
    assert(std::string(bcm.getName()) == "BCM");
    
    std::cout << "    testBCMBasicName passed" << std::endl;
}

void testBCMPotentiationAboveThreshold() {
    nlm::BCMRule bcm;
    bcm.setTheta(0.1f);
    bcm.setBCMLearningRate(0.01f);
    bcm.setBCMTimeConstant(20.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // High postsynaptic activity (above theta)
    // This should cause potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // High activity
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Should potentiate when above threshold
    assert(finalWeight > initialWeight);
    
    std::cout << "    testBCMPotentiationAboveThreshold passed (" 
              << initialWeight << " -> " << finalWeight << ")" << std::endl;
}

void testBCMDepressionBelowThreshold() {
    nlm::BCMRule bcm;
    bcm.setTheta(0.1f);
    bcm.setBCMLearningRate(0.01f);
    bcm.setBCMTimeConstant(20.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Low postsynaptic activity (below theta)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0};  // Low activity
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Should potentially depress or have minimal change
    // BCM depresses when below threshold
    assert(finalWeight <= initialWeight || std::abs(finalWeight - initialWeight) < 1e-5f);
    
    std::cout << "    testBCMDepressionBelowThreshold passed (" 
              << initialWeight << " -> " << finalWeight << ")" << std::endl;
}

void testBCMLearningBounds() {
    nlm::BCMRule bcm;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Strong potentiation attempt
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped
    assert(synapse.getWeight() <= 1.0f);
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testBCMLearningBounds passed" << std::endl;
}

void testBCMTargetProtection() {
    nlm::BCMRule bcm;
    bcm.setTheta(0.2f);
    bcm.setBCMLearningRate(0.01f);
    bcm.setBCMTimeConstant(50.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Simulate high activity followed by depression
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // High activity
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Should potentiate
    assert(synapse.getWeight() > 0.5f);
    
    // Now with very low activity (should not over-depress)
    preSpikes = {0.0};
    postSpikes = {5.0};  // Low activity
    
    float beforeDepression = synapse.getWeight();
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // BCM should protect against too much depression during high activity
    assert(synapse.getWeight() >= -0.5f);
    
    std::cout << "    testBCMTargetProtection passed (" 
              << beforeDepression << " -> " << synapse.getWeight() << ")" << std::endl;
}

void testBCMTimingWindow() {
    nlm::BCMRule bcm;
    bcm.setTheta(0.1f);
    bcm.setBCMLearningRate(0.01f);
    bcm.setBCMTimeConstant(30.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Very short timing difference - should have minimal effect
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0};  // Very close
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Small change for close timing
    assert(std::abs(finalWeight - initialWeight) < 0.05f);
    
    std::cout << "    testBCMTimingWindow passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMTimingWindowLarge() {
    nlm::BCMRule bcm;
    bcm.setTheta(0.1f);
    bcm.setBCMLearningRate(0.01f);
    bcm.setBCMTimeConstant(30.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Large timing difference - should have significant effect
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {100.0};  // Far apart
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Large timing difference should have less exponential effect
    // (exponential decay with large dt)
    assert(std::abs(finalWeight - initialWeight) < 0.02f);
    
    std::cout << "    testBCMTimingWindowLarge passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMNullSynapse() {
    nlm::BCMRule bcm;
    
    // Should not crash when synapse is null
    bcm.update(nullptr, {}, {}, 0.001);
    bcm.applyWeightChange(nullptr, 0.1f);
    
    std::cout << "    testBCMNullSynapse passed" << std::endl;
}

void runAll() {
    std::cout << "Running BCM tests..." << std::endl;
    testBCMCreation();
    testBCMConfiguration();
    testBCMMultipleConfigure();
    testBCMBasicName();
    testBCMPotentiationAboveThreshold();
    testBCMDepressionBelowThreshold();
    testBCMLearningBounds();
    testBCMTargetProtection();
    testBCMTimingWindow();
    testBCMTimingWindowLarge();
    testBCMNullSynapse();
}

} // namespace test_bcm