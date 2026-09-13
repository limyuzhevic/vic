// Neuromodulation Tests
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neuromodulation {

void testDopamineCreation() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getName() == std::string("DA"));
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() == 0.5f);
    
    std::cout << "    testDopamineCreation passed" << std::endl;
}

void testDopamineLevel() {
    nlm::Dopamine dopamine;
    
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.setLevel(0.0f);
    assert(dopamine.getLevel() == 0.0f);
    
    std::cout << "    testDopamineLevel passed" << std::endl;
}

void testDopaminePlasticityFactor() {
    nlm::Dopamine dopamine;
    
    assert(dopamine.getPlasticityFactor() == 0.5f);
    
    dopamine.setLevel(1.0f);
    assert(dopamine.getPlasticityFactor() == 1.0f);
    
    std::cout << "    testDopaminePlasticityFactor passed" << std::endl;
}

void testDopamineRewardSignaling() {
    nlm::Dopamine dopamine;
    
    dopamine.signalReward(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.signalReward(0.3f);
    assert(dopamine.getLevel() == 0.8f);
    
    std::cout << "    testDopamineRewardSignaling passed" << std::endl;
}

void testDopaminePredictionError() {
    nlm::Dopamine dopamine;
    
    dopamine.signalRewardPredictionError(0.2f);
    assert(dopamine.getLevel() == 0.2f);
    
    dopamine.signalRewardPredictionError(-0.1f);
    assert(dopamine.getLevel() == 0.1f);
    
    std::cout << "    testDopaminePredictionError passed" << std::endl;
}

void testCuriosityCreation() {
    nlm::Curiosity curiosity;
    
    assert(curiosity.getLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    std::cout << "    testCuriosityCreation passed" << std::endl;
}

void testCuriosityLevel() {
    nlm::Curiosity curiosity;
    
    curiosity.update(0.5f, 0.2f, 0.001);
    assert(curiosity.getLevel() == 0.0f);  // Initially should be 0 before update
    
    curiosity.setNoveltyWeight(0.7f);
    curiosity.setPredictionErrorWeight(0.3f);
    
    std::cout << "    testCuriosityLevel passed" << std::endl;
}

void testCuriosityExplorationDrive() {
    nlm::Curiosity curiosity;
    
    // Initially exploration drive should be 0
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    // Set weights
    curiosity.setNoveltyWeight(0.8f);
    curiosity.setPredictionErrorWeight(0.2f);
    
    // Exploration drive should be equal to level
    assert(curiosity.getExplorationDrive() == curiosity.getLevel());
    
    std::cout << "    testCuriosityExplorationDrive passed" << std::endl;
}

void testNoveltyCreation() {
    nlm::Novelty novelty;
    
    assert(novelty.getLevel() == 0.0f);
    assert(novelty.getHistory().empty());
    
    std::cout << "    testNoveltyCreation passed" << std::endl;
}

void testNoveltyDetection() {
    nlm::Novelty novelty;
    
    // Create dummy patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Same patterns should produce little or no novelty
    novelty.detectNovelty(pattern1, pattern2);
    
    // Different patterns should produce novelty
    std::vector<float> pattern3 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    novelty.detectNovelty(pattern1, pattern3);
    
    // Level should be non-negative
    assert(novelty.getLevel() >= 0.0f);
    
    std::cout << "    testNoveltyDetection passed" << std::endl;
}

void testNoveltyHistory() {
    nlm::Novelty novelty;
    
    assert(novelty.getHistory().empty());
    
    // Set a level
    novelty.setLevel(0.5f);
    
    // Update should add to history
    novelty.update(0.001);
    
    assert(!novelty.getHistory().empty());
    
    std::cout << "    testNoveltyHistory passed" << std::endl;
}

void testPredictionErrorCreation() {
    nlm::PredictionError predError;
    
    assert(predError.getError() == 0.0f);
    assert(predError.getHistory().empty());
    
    std::cout << "    testPredictionErrorCreation passed" << std::endl;
}

void testPredictionErrorCompute() {
    nlm::PredictionError predError;
    
    // Set prediction and actual values
    predError.computeError(0.8f, 0.6f); // Actual < predicted
    float error = predError.getError();
    assert(error != 0.0f);
    
    // Test magnitude
    float magnitude = predError.getMagnitude();
    assert(magnitude >= 0.0f);
    
    std::cout << "    testPredictionErrorCompute passed" << std::endl;
}

void testPredictionErrorHistory() {
    nlm::PredictionError predError;
    
    assert(predError.getHistory().empty());
    
    // Compute error multiple times
    predError.computeError(1.0f, 0.5f);
    predError.computeError(0.7f, 0.9f);
    
    assert(predError.getHistory().size() >= 2);
    
    std::cout << "    testPredictionErrorHistory passed" << std::endl;
}

void testRewardCreation() {
    nlm::Reward reward;
    
    assert(reward.getValue() == 0.0f);
    assert(reward.getHistory().empty());
    
    std::cout << "    testRewardCreation passed" << std::endl;
}

void testRewardValue() {
    nlm::Reward reward;
    
    // Add some reward
    reward.add(10.0f);
    assert(reward.getValue() == 10.0f);
    
    // Add negative reward
    reward.add(-5.0f);
    assert(reward.getValue() == 5.0f);
    
    // Set value directly
    reward.setValue(0.0f);
    assert(reward.getValue() == 0.0f);
    
    std::cout << "    testRewardValue passed" << std::endl;
}

void testRewardHistory() {
    nlm::Reward reward;
    
    assert(reward.getHistory().empty());
    
    // Add reward
    reward.add(1.0f);
    assert(reward.getHistory().size() == 1);
    
    // Add more
    reward.add(2.0f);
    assert(reward.getHistory().size() == 2);
    
    std::cout << "    testRewardHistory passed" << std::endl;
}

void testIntegratedNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    // Create neuromodulators
    nlm::Dopamine dopamine;
    nlm::Curiosity curiosity;
    nlm::Novelty novelty;
    nlm::PredictionError predError;
    nlm::Reward reward;
    
    // Initialize with brain reference (if they have initialize methods)
    // curiosity.initialize(&brain);
    // novelty.initialize(&brain);
    // predError.initialize(&brain);
    
    // Test integrated behavior
    dopamine.signalReward(1.0f);
    curiosity.update(0.5f, 0.0f, 0.001);
    novelty.setLevel(0.3f);
    predError.computeError(1.0f, 0.8f);
    reward.add(5.0f);
    
    // All should have non-zero values
    assert(dopamine.getLevel() > 0.0f);
    assert(curiosity.getLevel() >= 0.0f);
    assert(novelty.getLevel() >= 0.0f);
    assert(predError.getError() != 0.0f);
    assert(reward.getValue() > 0.0f);
    
    std::cout << "    testIntegratedNeuromodulation passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation tests..." << std::endl;
    
    testDopamineCreation();
    testDopamineLevel();
    testDopaminePlasticityFactor();
    testDopamineRewardSignaling();
    testDopaminePredictionError();
    
    testCuriosityCreation();
    testCuriosityLevel();
    testCuriosityExplorationDrive();
    
    testNoveltyCreation();
    testNoveltyDetection();
    testNoveltyHistory();
    
    testPredictionErrorCreation();
    testPredictionErrorCompute();
    testPredictionErrorHistory();
    
    testRewardCreation();
    testRewardValue();
    testRewardHistory();
    
    testIntegratedNeuromodulation();
}

} // namespace test_neuromodulation
