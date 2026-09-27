// Prediction System Tests
// Tests for Phase 6 prediction integration

#include "brain/Brain.hpp"
#include "prediction/PredictionSystem.hpp"
#include "sensory/SensoryInput.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_prediction {

void testPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predSys = brain.getPredictionSystem();
    if (!predSys) {
        std::cout << "    testPredictionSystem skipped (no prediction system)" << std::endl;
        return;
    }
    
    // Test prediction error
    float error = predSys->getPredictionError();
    assert(error >= 0.0f);
    
    // Test confidence
    float confidence = predSys->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test error history
    auto errorHistory = predSys->getErrorHistory();
    assert(errorHistory.size() > 0);
    
    // Test training
    nlm::SensoryInput observation;
    observation.vision = {0.5f, 0.3f, 0.2f, 0.1f, 0.0f};
    predSys->train(observation);
    
    // Clear history after training
    predSys->clearHistory();
    auto emptyHistory = predSys->getErrorHistory();
    assert(emptyHistory.size() == 0);
    
    std::cout << "    testPredictionSystem passed" << std::endl;
}

void testPredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 150);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predSys = brain.getPredictionSystem();
    if (!predSys) {
        std::cout << "    testPredictionIntegration skipped (no prediction system)" << std::endl;
        return;
    }
    
    // Test with different observation patterns
    for (int i = 0; i < 20; ++i) {
        nlm::SensoryInput observation;
        observation.vision = {
            static_cast<float>(i) % 5 * 0.2f,
            static_cast<float>(i) % 3 * 0.3f,
            static_cast<float>((i + 1) % 4) * 0.25f,
            static_cast<float>((i + 2) % 6) * 0.15f,
            0.5f
        };
        
        auto prediction = predSys->predictNextState(observation);
        if (prediction) {
            predSys->updatePredictions(*prediction, observation);
        }
        
        predSys->train(observation);
    }
    
    std::cout << "    testPredictionIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Prediction Tests..." << std::endl;
    testPredictionSystem();
    testPredictionIntegration();
}

} // namespace test_prediction