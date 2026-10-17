// Neuromodulation System Tests
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuromodulation {

void testNeuromodulator() {
    nlm::Neuromodulator neuromodulator;
    
    // Test initialization
    neuromodulator.initialize(nullptr); // Pass nullptr for brain pointer
    
    // Test baseline level
    float level = neuromodulator.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    // Test update
    neuromodulator.update(0.5f, 0.3f, 0.1f, 0.2f); // novelty, predictionError, reward, punishment
    
    // Test current values
    assert(neuromodulator.getNovelty() >= 0.0f);
    assert(neuromodulator.getPredictionError() >= 0.0f);
    assert(neuromodulator.getReward() >= 0.0f);
    assert(neuromodulator.getPunishment() >= 0.0f);
    
    // Test clamp functions
    neuromodulator.clampLevel(0.0f, 1.0f);
    neuromodulator.clampLevel(-0.5f, 2.0f); // Should clamp to [0, 1]
    
    std::cout << "    testNeuromodulator passed" << std::endl;
}

void testDopamine() {
    nlm::Dopamine dopamine;
    
    // Test initialization
    dopamine.initialize(nullptr);
    
    // Test baseline
    assert(dopamine.getLevel() >= 0.0f);
    
    // Test computation with different inputs
    dopamine.update(0.8f, 0.5f, 1.0f); // learningRate, predictionError, rewardPrediction
    
    // Test level
    float level = dopamine.getLevel();
    assert(level >= 0.0f);
    
    // Test factors
    assert(dopamine.getLearningRateFactor() >= 0.0f);
    assert(dopamine.getPredictionErrorFactor() >= 0.0f);
    assert(dopamine.getRewardPredictionFactor() >= 0.0f);
    
    std::cout << "    testDopamine passed" << std::endl;
}

void testCuriosity() {
    nlm::Curiosity curiosity;
    
    // Test initialization
    curiosity.initialize(nullptr);
    
    // Test baseline
    assert(curiosity.getLevel() >= 0.0f);
    assert(curiosity.getExplorationDrive() >= 0.0f);
    
    // Test update with novelty and prediction error
    curiosity.update(0.9f, 0.7f, 1.0f); // novelty, predictionError, dt
    
    // Test exploration drive
    float exploration = curiosity.getExplorationDrive();
    assert(exploration >= 0.0f);
    
    // Test parameter setting
    curiosity.setNoveltyWeight(0.8f);
    curiosity.setPredictionErrorWeight(0.9f);
    curiosity.setDecayRate(0.01f);
    
    assert(curiosity.getExplorationDrive() >= 0.0f);
    
    // Test reset
    curiosity.reset();
    assert(curiosity.getLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    std::cout << "    testCuriosity passed" << std::endl;
}

void testNovelty() {
    nlm::Novelty novelty;
    
    // Test initialization
    novelty.initialize(nullptr);
    
    // Test baseline
    assert(novelty.getLevel() >= 0.0f);
    assert(novelty.getNoveltyScore() >= 0.0f);
    assert(novelty.getSurpriseScore() >= 0.0f);
    
    // Test update with sensory input
    std::vector<float> inputPattern(10, 0.1f); // Small pattern = low novelty
    novelty.update(inputPattern, 0.5f); // pattern, dt
    
    // Test detection
    bool detected = novelty.detectNovelty();
    // Should detect novelty depending on the pattern
    
    // Test threshold setting
    novelty.setDetectionThreshold(0.5f);
    novelty.setDecayRate(0.1f);
    
    assert(novelty.getLevel() >= 0.0f);
    
    // Test reset
    novelty.reset();
    assert(novelty.getLevel() == 0.0f);
    assert(novelty.getNoveltyScore() == 0.0f);
    assert(novelty.getSurpriseScore() == 0.0f);
    
    std::cout << "    testNovelty passed" << std::endl;
}

void testPredictionError() {
    nlm::PredictionError predictionError;
    
    // Test initialization
    predictionError.initialize(nullptr);
    
    // Test baseline
    assert(predictionError.getLevel() >= 0.0f);
    assert(predictionError.getErrorMagnitude() >= 0.0f);
    
    // Test update with predicted and actual values
    std::vector<float> predicted(5, 0.5f);
    std::vector<float> actual(5, 0.8f);
    predictionError.update(predicted, actual, 1.0f); // predicted, actual, dt
    
    // Test error magnitude
    float error = predictionError.getErrorMagnitude();
    assert(error >= 0.0f);
    
    // Test threshold setting
    predictionError.setDetectionThreshold(0.1f);
    predictionError.setLearningRate(0.05f);
    
    assert(predictionError.getErrorMagnitude() >= 0.0f);
    
    // Test reset
    predictionError.reset();
    assert(predictionError.getLevel() == 0.0f);
    assert(predictionError.getErrorMagnitude() == 0.0f);
    
    std::cout << "    testPredictionError passed" << std::endl;
}

void testReward() {
    nlm::Reward reward;
    
    // Test initialization
    reward.initialize(nullptr);
    
    // Test baseline
    assert(reward.getLevel() >= 0.0f);
    assert(reward.getRewardValue() >= 0.0f);
    assert(reward.getPunishmentValue() >= 0.0f);
    
    // Test update with reward and punishment
    reward.update(1.0f, 0.5f, 1.0f); // reward, punishment, dt
    
    // Test values
    float rewardValue = reward.getRewardValue();
    float punishmentValue = reward.getPunishmentValue();
    
    assert(rewardValue >= 0.0f);
    assert(punishmentValue >= 0.0f);
    
    // Test parameter setting
    reward.setLearningRate(0.1f);
    reward.setMaxReward(10.0f);
    reward.setMaxPunishment(5.0f);
    
    assert(reward.getRewardValue() >= 0.0f);
    
    // Test reset
    reward.reset();
    assert(reward.getLevel() == 0.0f);
    assert(reward.getRewardValue() == 0.0f);
    assert(reward.getPunishmentValue() == 0.0f);
    
    std::cout << "    testReward passed" << std::endl;
}

void runAll() {
    testNeuromodulator();
    testDopamine();
    testCuriosity();
    testNovelty();
    testPredictionError();
    testReward();
}

} // namespace test_neuromodulation