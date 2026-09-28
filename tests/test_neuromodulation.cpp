// Neuromodulation System Unit Tests
// These tests verify the functionality of NLM's neuromodulation systems

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_neuromodulation {

void testDopamineSystem() {
    std::cout << "    testDopamineSystem..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Get dopamine system
    auto* dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    // Test initial state
    assert(dopamine->getLevel() == 0.0f); // Should start at 0
    
    // Apply reward modulation
    dopamine->applyReward(0.5f, 0.3f); // Reward of 0.5, predicted 0.3
    assert(dopamine->getLevel() > 0.0f); // Should increase due to prediction error
    
    // Apply penalty (negative reward)
    dopamine->applyReward(-0.2f, 0.1f); // Penalty of -0.2, predicted 0.1
    assert(dopamine->getLevel() < 0.0f); // Should decrease due to negative prediction error
    
    // Test reward prediction error calculation
    dopamine->setExpectedReward(0.8f);
    assert(dopamine->getExpectedReward() == 0.8f);
    
    std::cout << "    testDopamineSystem passed" << std::endl;
}

void testCuriositySystem() {
    std::cout << "    testCuriositySystem..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Get curiosity system
    auto* curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    // Test initial state
    assert(curiosity->getLevel() == 0.0f);
    
    // Test curiosity computation with high novelty
    curiosity->setNoveltyLevel(0.9f);
    curiosity->setPredictionError(0.5f);
    
    curiosity->updateCuriosity();
    
    // Should be high due to high novelty and prediction error
    assert(curiosity->getLevel() > 0.5f);
    
    // Test with low novelty and prediction error
    curiosity->setNoveltyLevel(0.1f);
    curiosity->setPredictionError(0.0f);
    
    curiosity->updateCuriosity();
    
    // Should be low
    assert(curiosity->getLevel() < 0.3f);
    
    // Test exploration behavior
    curiosity->setLevel(0.8f); // High curiosity
    assert(curiosity->shouldExplore() == true); // Should explore when curiosity is high
    
    curiosity->setLevel(0.1f); // Low curiosity
    assert(curiosity->shouldExplore() == false); // Should not explore when curiosity is low
    
    std::cout << "    testCuriositySystem passed" << std::endl;
}

void testNoveltySystem() {
    std::cout << "    testNoveltySystem..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Get novelty system
    auto* novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    // Test initial state
    assert(novelty->getLevel() == 0.0f);
    
    // Test novelty detection
    std::vector<float> currentPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> previousPattern = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    
    float noveltyLevel = novelty->computeNovelty(currentPattern, previousPattern);
    assert(noveltyLevel > 0.0f); // Should detect change
    
    // Test with identical patterns
    float sameNovelty = novelty->computeNovelty(currentPattern, currentPattern);
    assert(sameNovelty == 0.0f); // Should detect no change
    
    // Test novelty decay
    novelty->setLevel(0.8f);
    novelty->applyDecay(0.99f); // 1% decay
    assert(novelty->getLevel() < 0.8f); // Should have decayed
    
    // Test novelty threshold
    novelty->setLevel(0.05f);
    assert(novelty->isNovel() == false); // Below threshold
    
    novelty->setLevel(0.15f);
    assert(novelty->isNovel() == true); // Above threshold
    
    std::cout << "    testNoveltySystem passed" << std::endl;
}

void testPredictionErrorSystem() {
    std::cout << "    testPredictionErrorSystem..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Get prediction error system
    auto* predictionError = brain->getPredictionError();
    assert(predictionError != nullptr);
    
    // Test initial state
    assert(predictionError->getError() == 0.0f);
    
    // Test prediction error computation
    predictionError->updatePredictionError(0.8f, 0.5f); // Actual 0.8, predicted 0.5
    assert(predictionError->getError() > 0.0f); // Should be positive (actual > predicted)
    
    predictionError->updatePredictionError(0.3f, 0.6f); // Actual 0.3, predicted 0.6
    assert(predictionError->getError() < 0.0f); // Should be negative (actual < predicted)
    
    // Test error magnitude
    float error = predictionError->getError();
    assert(error >= -1.0f && error <= 1.0f); // Should be bounded
    
    std::cout << "    testPredictionErrorSystem passed" << std::endl;
}

void testRewardSystem() {
    std::cout << "    testRewardSystem..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Get reward system
    auto* reward = brain->getReward();
    assert(reward != nullptr);
    
    // Test initial state
    assert(reward->getValue() == 0.0f);
    
    // Test reward processing
    reward->processReward(1.0f, nlm::RewardType::Positive); // Positive reward
    assert(reward->getValue() > 0.0f);
    
    reward->processReward(-1.0f, nlm::RewardType::Negative); // Negative reward
    assert(reward->getValue() < 0.0f);
    
    // Test reward accumulation
    float accumulated = reward->getAccumulatedReward(0.1f); // 10% time constant
    assert(accumulated >= 0.0f);
    
    // Test reward discounting
    float discounted = reward->getDiscountedReward(0.5f); // 50% discount
    assert(discounted < reward->getValue()); // Should be discounted
    
    std::cout << "    testRewardSystem passed" << std::endl;
}

void testNeuromodulatorIntegration() {
    std::cout << "    testNeuromodulatorIntegration..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test all neuromodulator systems are integrated
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getNovelty() != nullptr);
    assert(brain->getPredictionError() != nullptr);
    assert(brain->getReward() != nullptr);
    
    // Test that neuromodulators affect brain behavior
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Apply some reward signal
        brain->applyRewardModulation(0.1f, 0.0f); // Small reward
        
        // Check that neuromodulators have levels
        assert(brain->getDopamine()->getLevel() >= 0.0f);
        assert(brain->getCuriosity()->getLevel() >= 0.0f);
        assert(brain->getNovelty()->getLevel() >= 0.0f);
        
        brain->step(step);
    }
    
    std::cout << "    testNeuromodulatorIntegration passed" << std::endl;
}

void runAllNeuromodulationTests() {
    std::cout << "=== Running Neuromodulation System Tests ===" << std::endl;
    
    testDopamineSystem();
    testCuriositySystem();
    testNoveltySystem();
    testPredictionErrorSystem();
    testRewardSystem();
    testNeuromodulatorIntegration();
    
    std::cout << "=== All Neuromodulation System Tests Passed ===" << std::endl;
}

} // namespace test_neuromodulation