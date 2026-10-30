// Neuromodulation Tests
// Tests for neuromodulation systems (Dopamine, Curiosity, Novelty, PredictionError)

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neuromodulation {

void testDopamine() {
    std::cout << "    Testing Dopamine Neuromodulation..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("dopamine_baseline", 0.1);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    // Test dopamine functionality
    float baseline = dopamine->getBaseline();
    assert(std::abs(baseline - 0.1f) < 0.001f);
    
    // Test reward signaling
    dopamine->signalReward(1.0f);
    float level_after_reward = dopamine->getLevel();
    assert(level_after_reward > baseline);
    
    // Test reward prediction error
    dopamine->signalRewardPredictionError(0.5f);
    float level_after_rpe = dopamine->getLevel();
    assert(level_after_rpe > 0.0f);
    
    // Test plasticity factor
    float plasticity_factor = dopamine->getPlasticityFactor();
    assert(plasticity_factor > 0.0f && plasticity_factor < 2.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void testCuriosity() {
    std::cout << "    Testing Curiosity Neuromodulation..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    // Test curiosity functionality
    float baseline = curiosity->getBaseline();
    
    // Test exploration drive
    curiosity->signalExploration(1.0f);
    float exploration_drive = curiosity->getExplorationDrive();
    assert(exploration_drive > baseline);
    
    std::cout << "      PASSED" << std::endl;
}

void testNovelty() {
    std::cout << "    Testing Novelty Detection..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    // Test novelty detection
    float novelty_level = novelty->getNoveltyLevel();
    
    // Test response to novel stimulus
    novelty->signalNovelty(0.8f);
    float response = novelty->getResponse();
    assert(response > 0.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void testPredictionError() {
    std::cout << "    Testing Prediction Error..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto prediction_error = brain->getPredictionErrorSignal();
    assert(prediction_error != nullptr);
    
    // Test prediction error signaling
    prediction_error->signalPredictionError(0.5f);
    float error_level = prediction_error->getErrorLevel();
    assert(error_level > 0.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void runAll() {
    testDopamine();
    testCuriosity();
    testNovelty();
    testPredictionError();
}

} // namespace test_neuromodulation
