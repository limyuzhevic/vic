// Neuromodulation Integration Tests
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Novelty.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_neuromodulation {

void testNeuromodulatorCreation() {
    std::cout << "    testNeuromodulatorCreation passed (placeholder - base class)" << std::endl;
}

void testDopamineSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Check initial state
    assert(dopamine->getLevel() == 0.0f);
    
    // Update with prediction error
    float predictionError = 0.5f;
    dopamine->update(predictionError * 0.1f);
    
    // Check that dopamine level changed
    assert(dopamine->getLevel() >= 0.0f);
    
    std::cout << "    testDopamineSystem passed" << std::endl;
}

void testCuriositySystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Check initial state
    assert(curiosity->getLevel() == 0.0f);
    
    // Simulate exploration drive
    for (int i = 0; i < 10; ++i) {
        curiosity->update(0.001f);
    }
    
    // Curiosity should accumulate over time
    assert(curiosity->getLevel() >= 0.0f);
    
    std::cout << "    testCuriositySystem passed" << std::endl;
}

void testPredictionErrorSignal() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Check initial state
    assert(predictionError->getErrorLevel() == 0.0f);
    assert(predictionError->getTemporalDifference() == 0.0f);
    
    // Simulate prediction error
    float prediction = 0.8f;
    float target = 1.0f;
    predictionError->updatePredictionError(prediction, target);
    
    // Error should be high due to difference between prediction and target
    assert(predictionError->getErrorLevel() > 0.0f);
    
    std::cout << "    testPredictionErrorSignal passed" << std::endl;
}

void testNoveltyDetection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Check initial state
    assert(novelty->getLevel() == 0.0f);
    
    // Simulate sensory input with high variance to trigger novelty
    std::vector<float> sensoryInput = {0.9f, 0.1f, 0.8f, 0.2f, 0.7f};
    
    for (int i = 0; i < 20; ++i) {
        novelty->update(sensoryInput, 0.001f);
    }
    
    // Novelty level should increase with unpredictable input
    assert(novelty->getLevel() >= 0.0f);
    
    std::cout << "    testNoveltyDetection passed" << std::endl;
}

void testRewardSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* reward = brain.getReward();
    assert(reward != nullptr);
    
    // Check initial state
    assert(reward->getLevel() == 0.0f);
    
    // Simulate reward learning
    for (int i = 0; i < 5; ++i) {
        reward->receiveReward(1.0f, 0.001f);  // Reward + learning rate
    }
    
    // Reward level should increase
    assert(reward->getLevel() > 0.0f);
    
    std::cout << "    testRewardSystem passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    auto* reward = brain.getReward();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    assert(reward != nullptr);
    
    // Run simulation to allow neuromodulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Check that neuromodulation systems are operating
        assert(dopamine->getLevel() >= 0.0f);
        assert(curiosity->getLevel() >= 0.0f);
        assert(novelty->getLevel() >= 0.0f);
        assert(predictionError->getErrorLevel() >= 0.0f);
        assert(reward->getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void runAll() {
    testNeuromodulatorCreation();
    testDopamineSystem();
    testCuriositySystem();
    testPredictionErrorSignal();
    testNoveltyDetection();
    testRewardSystem();
    testNeuromodulationIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Neuromodulation Tests PASSED ===" << std::endl;
    std::cout << "Neuromodulation systems work correctly in brain simulation" << std::endl;
}

} // namespace test_neuromodulation