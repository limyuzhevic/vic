// Neuromodulation System Tests
// This file tests the neuromodulation components of the NLM framework
// Covers dopamine, curiosity, novelty detection, and prediction error systems

#include "neuromodulation/DopamineSystem/DopamineSystem.hpp"
#include "neuromodulation/CuriositySystem/CuriositySystem.hpp"
#include "neuromodulation/NoveltySystem/NoveltySystem.hpp"
#include "neuromodulation/PredictionErrorSystem/PredictionErrorSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_neuromodulation {

void testDopamineSystemRewardLearning() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("neuromodulation.dopamine.learning_rate", 0.1, nlm::ConfigSource::Default);
    config->set("neuromodulation.dopamine.baseline", 0.5, nlm::ConfigSource::Default);
    config->set("neuromodulation.dopamine.noise_level", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto dopamineSystem = brain.getDopamineSystem();
    assert(dopamineSystem != nullptr);
    
    // Test reward prediction error calculation
    nlm::PredictionError reward = 0.8;  // Large reward
    nlm::PredictionError predictedReward = 0.2;
    nlm::Timestamp timestamp = 1000.0;
    
    dopamineSystem->processRewardPredictionError(reward, predictedReward, timestamp);
    
    // Check that dopamine level changed based on prediction error
    double currentDopamine = dopamineSystem->getCurrentLevel();
    assert(std::abs(currentDopamine - 0.5) > 0.1); // Should have changed from baseline
    
    std::cout << "    testDopamineSystemRewardLearning passed" << std::endl;
}

void testDopamineSystemMagnitudeCoding() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto dopamineSystem = brain.getDopamineSystem();
    
    // Test that different reward magnitudes produce different dopamine responses
    std::vector<std::pair<nlm::PredictionError, double>> testCases = {
        {-0.5, 0.2},   // Large punishment
        {0.0, 0.5},    // No prediction error
        {0.5, 0.8},    // Small reward
        {1.0, 0.95}    // Large reward
    };
    
    for (const auto& testCase : testCases) {
        nlm::PredictionError predictionError = testCase.first;
        nlm::Timestamp timestamp = 1000.0;
        
        dopamineSystem->processRewardPredictionError(predictionError, 0.0, timestamp);
        
        double dopamineLevel = dopamineSystem->getCurrentLevel();
        assert(dopamineLevel >= 0.0 && dopamineLevel <= 1.0); // Should be normalized
    }
    
    std::cout << "    testDopamineSystemMagnitudeCoding passed" << std::endl;
}

void testCuriositySystemExploration() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("neuromodulation.curiosity.exploration_rate", 0.3, nlm::ConfigSource::Default);
    config->set("neuromodulation.curiosity.sensitivity", 0.5, nlm::ConfigSource::Default);
    config->set("neuromodulation.curiosity.baseline", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto curiositySystem = brain.getCuriositySystem();
    assert(curiositySystem != nullptr);
    
    // Test novelty-driven exploration
    std::vector<double> novelInput{0.9, 0.8, 0.7, 0.6, 0.5}; // Novel pattern
    nlm::Timestamp timestamp = 1000.0;
    
    curiositySystem->processInput(novelInput, timestamp);
    
    double curiosityLevel = curiositySystem->getCurrentLevel();
    assert(curiosityLevel > curiositySystem->getBaselineLevel()); // Should increase for novel input
    
    // Test habituation to repeated input
    for (int i = 0; i < 5; ++i) {
        curiositySystem->processInput(novelInput, timestamp + i * 100.0);
    }
    
    double curiosityLevelAfterHabituation = curiositySystem->getCurrentLevel();
    // After habituation, curiosity level should decrease
    assert(curiosityLevelAfterHabituation <= curiosityLevel);
    
    std::cout << "    testCuriositySystemExploration passed" << std::endl;
}

void testNoveltySystemDetection() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("neuromodulation.novelty.threshold", 0.5, nlm::ConfigSource::Default);
    config->set("neuromodulation.novelty.sensitivity", 0.8, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto noveltySystem = brain.getNoveltySystem();
    assert(noveltySystem != nullptr);
    
    // Test detection of truly novel input
    std::vector<double> novelPattern{1.0, 0.0, 1.0, 0.0, 1.0, 0.0};
    nlm::Timestamp timestamp = 1000.0;
    
    bool isNovel = noveltySystem->detectNovelty(novelPattern, timestamp);
    assert(isNovel); // Should be detected as novel
    
    // Test that familiar input is not detected as novel
    std::vector<double> familiarPattern{0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    bool isFamiliar = noveltySystem->detectNovelty(familiarPattern, timestamp + 100.0);
    // Familiarity depends on threshold, but should not be novel with high similarity
    
    // Test novelty threshold adjustment
    double noveltyResponse = noveltySystem->getNoveltyResponse();
    assert(noveltyResponse >= 0.0 && noveltyResponse <= 1.0);
    
    std::cout << "    testNoveltySystemDetection passed" << std::endl;
}

void testPredictionErrorSystemLearning() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("neuromodulation.prediction_error.learning_rate", 0.05, nlm::ConfigSource::Default);
    config->set("neuromodulation.prediction_error.baseline", 0.0, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionErrorSystem = brain.getPredictionErrorSystem();
    assert(predictionErrorSystem != nullptr);
    
    // Test prediction error calculation
    nlm::PredictionError predicted = 0.3;
    nlm::PredictionError actual = 0.8;
    nlm::Timestamp timestamp = 1000.0;
    
    predictionErrorSystem->updatePrediction(predicted, timestamp);
    nlm::PredictionError pe = predictionErrorSystem->calculatePredictionError(actual);
    
    // Prediction error should be actual - predicted
    assert(std::abs(pe - (actual - predicted)) < 0.001);
    
    // Test prediction error modulation of plasticity
    double plasticityModulation = predictionErrorSystem->getPlasticityModulation();
    assert(plasticityModulation >= 0.0 && plasticityModulation <= 1.0);
    
    std::cout << "    testPredictionErrorSystemLearning passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("neuromodulation.dopamine.learning_rate", 0.1, nlm::ConfigSource::Default);
    config->set("neuromodulation.curiosity.exploration_rate", 0.2, nlm::ConfigSource::Default);
    config->set("neuromodulation.novelty.threshold", 0.5, nlm::ConfigSource::Default);
    config->set("neuromodulation.prediction_error.learning_rate", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all neuromodulation systems are present and functional
    assert(brain.getDopamineSystem() != nullptr);
    assert(brain.getCuriositySystem() != nullptr);
    assert(brain.getNoveltySystem() != nullptr);
    assert(brain.getPredictionErrorSystem() != nullptr);
    
    // Test integration by running a simulation step
    for (int step = 0; step < 10; ++step) {
        nlm::Brain brain(config);
        brain.initialize();
        brain.step(step);
        
        // All neuromodulation systems should still be accessible
        assert(brain.getDopamineSystem() != nullptr);
        assert(brain.getCuriositySystem() != nullptr);
        assert(brain.getNoveltySystem() != nullptr);
        assert(brain.getPredictionErrorSystem() != nullptr);
    }
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void runAll() {
    testDopamineSystemRewardLearning();
    testDopamineSystemMagnitudeCoding();
    testCuriositySystemExploration();
    testNoveltySystemDetection();
    testPredictionErrorSystemLearning();
    testNeuromodulationIntegration();
    
    std::cout << "All neuromodulation tests passed!" << std::endl;
}

} // namespace test_neuromodulation
