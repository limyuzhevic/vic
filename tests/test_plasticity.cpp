// Plasticity System Tests
// Tests for STDP, reward-modulated STDP, Hebbian learning, anti-Hebbian, and homeostatic plasticity

#include "plasticity/STDP.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/AntiHebbian.hpp"
#include "plasticity/HomeostaticPlasticity.hpp"
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

namespace test_plasticity {

void testEnhancedSTDP() {
    nlm::STDP stdp;
    
    // Test initial STDP parameters
    assert(stdp.getLTPWeight() > 0.0f);
    assert(stdp.getLTDWeight() > stdp.getLTPWeight());
    assert(stdp.getTimeConstantPlus() > 0.0f);
    assert(stdp.getTimeConstantMinus() > stdp.getTimeConstantPlus());
    
    // Test parameter configuration
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstantPlus(25.0f);
    stdp.setTimeConstantMinus(35.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstantPlus() == 25.0f);
    assert(stdp.getTimeConstantMinus() == 35.0f);
    
    // Test update method with spike timing
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Pre before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0}; // 5ms after pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() > 0.5f); // Should have potentiated
    
    // Post before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> postFirstSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> preFirstSpikes = {5.0, 15.0}; // 5ms before post
    
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    stdp.update(&synapse2, preFirstSpikes, postFirstSpikes, 0.001);
    assert(synapse2.getWeight() < 0.5f); // Should have depressed
    
    // Test empty spike vectors
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse3.setType(nlm::SynapseType::Excitatory);
    synapse3.setWeight(0.5f);
    
    float weightBeforeEmpty = synapse3.getWeight();
    stdp.update(&synapse3, {}, {1.0}, 0.001);
    assert(synapse3.getWeight() == weightBeforeEmpty);
    
    stdp.update(&synapse3, {1.0}, {}, 0.001);
    assert(synapse3.getWeight() == weightBeforeEmpty);
    
    std::cout << "    testEnhancedSTDP passed" << std::endl;
}

void testRewardModulatedSTDP() {
    nlm::RewardModulatedSTDP rmSTDP;
    
    // Test initial parameters
    assert(rmSTDP.getLTPWeight() > 0.0f);
    assert(rmSTDP.getDopamineLevel() == 0.0f);
    assert(rmSTDP.getRewardPredictionError() == 0.0f);
    
    // Test dopamine integration
    rmSTDP.setDopamineLevel(0.8f);
    assert(rmSTDP.getDopamineLevel() == 0.8f);
    
    rmSTDP.setRewardPredictionError(0.5f); // Positive RPE
    assert(rmSTDP.getRewardPredictionError() == 0.5f);
    
    // Test credit assignment window
    rmSTDP.setCreditAssignmentWindow(30.0f);
    assert(rmSTDP.getCreditAssignmentWindow() == 30.0f);
    
    // Test eligibility trace
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialTrace = rmSTDP.getTraceValue(&synapse);
    assert(initialTrace == 0.0f);
    
    rmSTDP.updateEligibilityTrace(&synapse, 0.1f, 0.001);
    float traceAfter = rmSTDP.getTraceValue(&synapse);
    assert(traceAfter > 0.0f);
    
    // Test trace decay
    rmSTDP.decayAllTraces(0.1f);
    float traceDecayed = rmSTDP.getTraceValue(&synapse);
    assert(traceDecayed < traceAfter);
    
    // Test configuration with dopamine
    rmSTDP.configure(0.03f, 0.035f, 20.0f, 30.0f, 0.7f, 0.5f);
    assert(rmSTDP.getLTPWeight() == 0.03f);
    assert(rmSTDP.getDopamineLevel() == 0.7f);
    assert(rmSTDP.getRewardPredictionError() == 0.5f);
    
    std::cout << "    testRewardModulatedSTDP passed" << std::endl;
}

void testHebbianLearningBCM() {
    nlm::Hebbian hebbian;
    
    // Test BCM theory parameters
    assert(hebbian.getTheta() > 0.0f && hebbian.getTheta() <= 1.0f);
    assert(hebbian.getBCMLearning() > 0.0f);
    assert(hebbian.getOjaAlpha() > 0.0f && hebbian.getOjaAlpha() <= 1.0f);
    
    // Test parameter configuration
    hebbian.configureBCM(0.6f, 0.02f); // theta = 0.6, BCM learning rate = 0.02
    assert(hebbian.getTheta() == 0.6f);
    assert(hebbian.getBCMLearning() == 0.02f);
    
    // Test Oja's rule
    hebbian.configureOja(0.4f); // Oja alpha = 0.4
    assert(hebbian.getOjaAlpha() == 0.4f);
    
    // Test covariance learning
    hebbian.configureCovariance(0.01f, 1000); // rate = 0.01, history size = 1000
    assert(hebbian.getCovarianceLearningRate() == 0.01f);
    assert(hebbian.getActivityHistorySize() == 1000);
    
    // Test Hebbian update with spike timing
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change based on Hebbian learning
    float newWeight = synapse.getWeight();
    assert(std::isfinite(newWeight));
    
    // Test activity history
    assert(hebbian.getAveragePreActivity(&synapse) > 0.0f);
    assert(hebbian.getAveragePostActivity(&synapse) > 0.0f);
    
    // Test covariance calculation
    float covariance = hebbian.getCovariance(&synapse);
    assert(std::isfinite(covariance));
    
    // Test modulation factor
    hebbian.setModulationFactor(2.0f); // Double the learning rate
    assert(hebbian.getModulationFactor() == 2.0f);
    
    std::cout << "    testHebbianLearningBCM passed" << std::endl;
}

void testAntiHebbianLearning() {
    nlm::AntiHebbian antiHebbian;
    
    // Test anti-Hebbian parameters
    assert(antiHebbian.getLearningRate() > 0.0f && antiHebbian.getLearningRate() <= 0.1f);
    assert(antiHebbian.getDepressionStrength() > 0.0f && antiHebbian.getDepressionStrength() <= 1.0f);
    assert(antiHebbian.getMinimumWeight() < 0.0f);
    assert(antiHebbian.getMaximumWeight() > 0.0f && antiHebbian.getMaximumWeight() <= 0.1f);
    
    // Test parameter configuration
    antiHebbian.configure(0.05f, 0.2f); // learningRate = 0.05, depressionStrength = 0.2
    assert(antiHebbian.getLearningRate() == 0.05f);
    assert(antiHebbian.getDepressionStrength() == 0.2f);
    
    // Test anti-Hebbian bounds
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Inhibitory); // Anti-Hebbian is for inhibitory synapses
    synapse.setWeight(0.0f); // Start at zero
    
    // Coactivity should trigger depression (weight decrease)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    antiHebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // For anti-Hebbian, weight should decrease with coactivity
    float newWeight = synapse.getWeight();
    assert(newWeight < 0.0f); // Should be more negative (stronger inhibition)
    
    // Test coactivity threshold
    antiHebbian.setCoactivityThreshold(0.5f);
    assert(antiHebbian.getCoactivityThreshold() == 0.5f);
    
    // Test anti-Hebbian learning rate
    antiHebbian.setAntiHebbianLearningRate(0.08f);
    assert(antiHebbian.getAntiHebbianLearningRate() == 0.08f);
    
    // Test weight bounds
    antiHebbian.setMinimumWeight(-0.8f);
    antiHebbian.setMaximumWeight(-0.01f);
    assert(antiHebbian.getMinimumWeight() == -0.8f);
    assert(antiHebbian.getMaximumWeight() == -0.01f);
    
    std::cout << "    testAntiHebbianLearning passed" << std::endl;
}

void testHomeostaticPlasticity() {
    nlm::HomeostaticPlasticity homeostatic;
    
    // Test homeostatic parameters
    assert(homeostatic.getTargetFiringRate() > 0.0f);
    assert(homeostatic.getHomeostaticLearningRate() > 0.0f);
    assert(homeostatic.getMetaplasticityRate() >= 0.0f);
    assert(homeostatic.getScalingExponent() > 0.1f && homeostatic.getScalingExponent() <= 2.0f);
    
    // Test parameter configuration
    homeostatic.configure(10.0f, 0.001f); // targetRate = 10.0 Hz, learningRate = 0.001
    assert(homeostatic.getTargetFiringRate() == 10.0f);
    assert(homeostatic.getHomeostaticLearningRate() == 0.001f);
    
    // Test activity history size
    homeostatic.setActivityHistorySize(200);
    assert(homeostatic.getActivityHistorySize() == 200);
    
    // Test scaling exponent
    homeostatic.setScalingExponent(1.5f);
    assert(homeostatic.getScalingExponent() == 1.5f);
    
    // Test homeostatic update
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0, 40.0, 50.0}; // High activity
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0, 45.0, 55.0};
    
    homeostatic.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should adapt towards homeostatic target
    float newWeight = synapse.getWeight();
    assert(std::isfinite(newWeight));
    
    // Test activity estimation
    float estimatedActivity = homeostatic.estimateNetworkActivity(&synapse, preSpikes);
    assert(estimatedActivity > 0.0f);
    
    // Test metaplasticity
    float runningRate = homeostatic.getMetaplasticityRate();
    assert(runningRate >= 0.0f);
    
    std::cout << "    testHomeostaticPlasticity passed" << std::endl;
}

void runAll() {
    std::cout << "Running Plasticity Tests..." << std::endl;
    testEnhancedSTDP();
    testRewardModulatedSTDP();
    testHebbianLearningBCM();
    testAntiHebbianLearning();
    testHomeostaticPlasticity();
    
    std::cout << "\nAll Plasticity Tests passed!" << std::endl;
}

} // namespace test_plasticity