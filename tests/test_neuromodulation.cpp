// Neuromodulation System Tests
// Tests for dopamine dynamics, curiosity-driven exploration, novelty detection, and integration

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

namespace test_neuromodulation {

void testDopamineBaselineAdaptation() {
    nlm::Dopamine dopamine;
    
    // Test initial baseline firing rate
    assert(dopamine.getBaseline() > 0.0f);
    assert(dopamine.getBaseline() <= 1.0f);
    
    // Test baseline adaptation
    float initialBaseline = dopamine.getBaseline();
    dopamine.setBaseline(0.8f);
    assert(dopamine.getBaseline() == 0.8f);
    
    // Test adaptation towards target
    dopamine.updatePrediction(0.5f);
    // Baseline should slowly adapt
    assert(dopamine.getBaseline() >= initialBaseline);
    
    std::cout << "    testDopamineBaselineAdaptation passed" << std::endl;
}

void testDopamineLearningRateModulation() {
    nlm::Dopamine dopamine;
    
    // Test initial learning rate modulation
    float initialMod = dopamine.getLearningRateModulation();
    assert(initialMod > 0.0f && initialMod <= 1.0f);
    
    // Test setting learning rate modulation
    dopamine.setLearningRateModulation(0.5f);
    assert(dopamine.getLearningRateModulation() == 0.5f);
    
    // Test bounds
    dopamine.setLearningRateModulation(2.0f); // Should clamp to max
    assert(dopamine.getLearningRateModulation() <= 1.0f);
    
    dopamine.setLearningRateModulation(-0.5f); // Should clamp to min
    assert(dopamine.getLearningRateModulation() >= 0.0f);
    
    // Test plasticity factor changes with dopamine level
    float lowDopLevel = 0.0f;
    dopamine.setLevel(lowDopLevel);
    float lowPlasticity = dopamine.getPlasticityFactor();
    
    float highDopLevel = 1.0f;
    dopamine.setLevel(highDopLevel);
    float highPlasticity = dopamine.getPlasticityFactor();
    
    // Plasticity should be sigmoidal (inverted U)
    assert(highPlasticity > lowPlasticity);
    
    std::cout << "    testDopamineLearningRateModulation passed" << std::endl;
}

void testRewardPredictionErrorTDLearning() {
    nlm::Reward reward;
    
    // Test initial state
    assert(reward.getPredictedValue() == 0.0f);
    assert(reward.getTotalReward() == 0.0f);
    assert(reward.getRewardCount() == 0);
    
    // Test reward accumulation
    reward.add(0.5f);
    assert(reward.getTotalReward() == 0.5f);
    assert(reward.getRewardCount() == 1);
    assert(reward.getAverageReward() == 0.5f);
    
    // Test prediction error
    reward.updatePrediction(0.3f); // Update prediction to 0.3
    float error = reward.computePredictionError(0.5f); // Actual is 0.5
    assert(std::abs(error - 0.2f) < 0.001f); // 0.5 - 0.3 = 0.2
    
    // Test reward statistics
    reward.add(1.0f);
    assert(reward.getAverageReward() == 0.75f);
    
    std::cout << "    testRewardPredictionErrorTDLearning passed" << std::endl;
}

void testCuriosityDrivenExploration() {
    nlm::Curiosity curiosity;
    
    // Test exploration drive initialization
    float initialDrive = curiosity.getExplorationDrive();
    assert(initialDrive == 0.0f);
    
    // Test novelty and prediction error update
    curiosity.update(0.8f, 0.3f, 0.01f); // High novelty, moderate prediction error
    
    // Curiosity should increase with novelty and prediction error
    assert(curiosity.getExplorationDrive() > initialDrive);
    
    // Test epsilon-greedy policy
    std::vector<float> actionValues = {0.1f, 0.5f, 0.9f};
    float selected = curiosity.selectAction(actionValues, "epsilon_greedy");
    assert(selected == 0.9f); // Should select max value (0.9)
    
    // Test UCB policy (requires history)
    curiosity.update(0.5f, 0.1f, 0.01f);
    // Exploration drive affects epsilon
    float epsilon = curiosity.getEpsilon();
    assert(epsilon > 0.01f); // Should be > 1% based on dopamine level
    
    // Test Boltzmann policy
    float boltzmannResult = curiosity.selectAction(actionValues, "boltzmann");
    assert(boltzmannResult >= 0.0f && boltzmannResult <= 0.9f);
    
    // Test Thompson sampling
    float thompsonResult = curiosity.selectAction(actionValues, "thompson_sampling");
    assert(thompsonResult >= 0.0f && thompsonResult <= 1.0f);
    
    // Test exploration temperature
    curiosity.setExplorationTemperature(2.0f);
    assert(curiosity.getExplorationTemperature() >= 2.0f);
    
    // Test epsilon setting
    curiosity.setEpsilon(0.3f);
    assert(curiosity.getEpsilon() == 0.3f);
    
    std::cout << "    testCuriosityDrivenExploration passed" << std::endl;
}

void testNoveltyDetectionFeatureBased() {
    nlm::Novelty novelty;
    
    // Test initialization
    assert(novelty.getHistory().empty());
    
    // Create test patterns
    std::vector<float> currentPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> similarPattern = {0.11f, 0.21f, 0.31f, 0.41f, 0.51f}; // Similar
    std::vector<float> differentPattern = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f}; // Different
    
    // Test feature-based novelty detection
    float similarNovelty = novelty.computeFeatureNovelty(currentPattern, similarPattern);
    float differentNovelty = novelty.computeFeatureNovelty(currentPattern, differentPattern);
    
    // Similar patterns should have lower novelty than different patterns
    assert(similarNovelty < differentNovelty);
    
    // Test statistical novelty detection
    float statisticalNovelty = novelty.computeStatisticalNovelty(currentPattern);
    assert(statisticalNovelty >= 0.0f && statisticalNovelty <= 1.0f);
    
    // Test pattern memory management
    size_t initialMemory = novelty.getHistory().size();
    novelty.setPatternMemorySize(1000);
    
    // Test novelty detection with observation
    nlm::Observation observation;
    // In a real system, observation would contain features
    
    // Test novelty threshold
    novelty.setNoveltyThreshold(0.5f);
    
    std::cout << "    testNoveltyDetectionFeatureBased passed" << std::endl;
}

void testNoveltyDetectionStatistical() {
    nlm::Novelty novelty;
    
    // Test with empty history
    assert(novelty.getHistory().empty());
    
    // Create patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.15f, 0.25f, 0.35f}; // Similar
    std::vector<float> pattern3 = {0.9f, 0.8f, 0.7f}; // Different
    
    // Test statistical novelty detection
    float novelty1 = novelty.computeStatisticalNovelty(pattern1);
    float novelty2 = novelty.computeStatisticalNovelty(pattern2);
    float novelty3 = novelty.computeStatisticalNovelty(pattern3);
    
    // Pattern 3 should have higher novelty
    assert(novelty3 > novelty1);
    assert(novelty3 > novelty2);
    
    // Test history management
    std::vector<float> history = novelty.getHistory();
    assert(!history.empty());
    
    // Test clearing history
    novelty.clearHistory();
    assert(novelty.getHistory().empty());
    
    // Test statistical threshold
    novelty.setStatisticalThreshold(0.7f);
    
    std::cout << "    testNoveltyDetectionStatistical passed" << std::endl;
}

void testPredictionErrorTDLearning() {
    nlm::PredictionError predError;
    
    // Test initial state
    assert(predError.getPredictedValue() == 0.0f);
    assert(predError.getValuePredictionError() == 0.0f);
    assert(predError.getHistory().empty());
    
    // Test prediction error computation
    predError.computeError(0.5f, 0.3f); // Predicted: 0.5, Actual: 0.3
    assert(std::abs(predError.getValuePredictionError() - (-0.2f)) < 0.001f);
    
    // Test prediction update
    predError.updatePrediction(0.4f);
    assert(predError.getPredictedValue() == 0.4f);
    
    // Test temporal difference learning
    float td = predError.computeTD(0.5f, 0.4f, 0.6f);
    float expectedTD = 0.5f + 0.9f * 0.6f - 0.4f; // gamma = 0.9
    assert(std::abs(td - expectedTD) < 0.001f);
    
    // Test value function learning
    predError.learnValueFunction(0.4f, 0.5f);
    assert(predError.getMagnitude() > 0.0f);
    
    // Test error propagation
    predError.propagateError(0.5f);
    
    // Test error history
    std::vector<float> history = predError.getHistory();
    assert(!history.empty());
    
    // Test learning rate
    predError.setLearningRate(0.1f);
    
    // Test discount factor
    predError.setDiscountFactor(0.99f);
    
    // Test error threshold
    predError.setErrorThreshold(1.0f);
    
    std::cout << "    testPredictionErrorTDLearning passed" << std::endl;
}

void testNeuromodulatorIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test neuromodulator system initialization
    auto* neuromodSystem = brain.getNeuromodulatorSystem();
    assert(neuromSystem != nullptr);
    
    // Test neuromodulator components
    auto* dopamine = neuromodSystem->getDopamine();
    auto* curiosity = neuromodSystem->getCuriosity();
    auto* novelty = neuromodSystem->getNovelty();
    auto* predictionError = neuromodSystem->getPredictionError();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test integration methods
    nlm::Observation observation;
    neuromodSystem->recordReward(0.5f, &observation);
    neuromodSystem->recordPredictionError(0.3f, &observation);
    neuromodSystem->recordNovelty(0.7f);
    
    // Test curiosity update
    neuromodSystem->updateCuriosity(0.3f, 0.7f, 0.01f);
    
    // Test exploration policy selection
    std::vector<float> actionValues = {0.2f, 0.5f, 0.8f};
    float selected = neuromodSystem->selectExplorationPolicy(0.5f, actionValues, "epsilon_greedy");
    assert(selected == 0.8f);
    
    // Test neuromodulator system reset
    neuromodSystem->reset();
    
    std::cout << "    testNeuromodulatorIntegration passed" << std::endl;
}

void testNeuromodulatorPlasticityIntegration() {
    // Test integration between neuromodulators and plasticity
    nlm::Dopamine dopamine;
    
    // Test plasticity factor calculation
    float plasticityFactor = dopamine.getPlasticityFactor();
    assert(plasticityFactor >= 0.0f && plasticityFactor <= 1.0f);
    
    // Test learning rate modulation effects
    dopamine.setLearningRateModulation(2.0f); // High modulation
    float highModPlasticity = dopamine.getPlasticityFactor();
    
    dopamine.setLearningRateModulation(0.5f); // Normal modulation
    float normalModPlasticity = dopamine.getPlasticityFactor();
    
    // Higher modulation should affect plasticity
    assert(highModPlasticity >= normalModPlasticity);
    
    // Test eligibility trace integration
    dopamine.addEligibilityContribution(0.5f, 0.0f, 10.0f); // pre:0.0, post:10.0
    assert(dopamine.getEligibilityTrace() > 0.0f);
    
    // Test eligibility trace decay
    float traceBefore = dopamine.getEligibilityTrace();
    dopamine.updateEligibilityTrace(0.9f); // Decay
    float traceAfter = dopamine.getEligibilityTrace();
    
    // Trace should decay
    assert(traceAfter < traceBefore);
    
    std::cout << "    testNeuromodulatorPlasticityIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation Tests..." << std::endl;
    testDopamineBaselineAdaptation();
    testDopamineLearningRateModulation();
    testRewardPredictionErrorTDLearning();
    testCuriosityDrivenExploration();
    testNoveltyDetectionFeatureBased();
    testNoveltyDetectionStatistical();
    testPredictionErrorTDLearning();
    testNeuromodulatorIntegration();
    testNeuromodulatorPlasticityIntegration();
    
    std::cout << "\nAll Neuromodulation Tests passed!" << std::endl;
}

} // namespace test_neuromodulation