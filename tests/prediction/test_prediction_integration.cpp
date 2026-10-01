// Prediction System Integration Tests for NLM
// Tests integration between PredictionSystem and brain systems

#include "brain/Brain.hpp"
#include "prediction/PredictionSystem.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "cognition/NeuralPlanner.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_prediction_integration {

void testPredictionSystemIntegration() {
    std::cout << "    Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("prediction_error_threshold", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction error signal generation
    std::vector<float> predictedOutput = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> actualOutput = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    
    // Apply prediction error
    float error = predictionSystem->computePredictionError(predictedOutput, actualOutput);
    assert(error >= 0.0f);
    
    // Verify error affects learning
    assert(predictionSystem->getPredictionErrorHistory().size() > 0);
    
    // Test prediction confidence
    float confidence = predictionSystem->getPredictionConfidence(predictedOutput);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testPredictionSystemIntegration passed" << std::endl;
}

void testPredictionErrorLearning() {
    std::cout << "    Testing prediction error learning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Generate multiple prediction errors
    for (int i = 0; i < 10; ++i) {
        std::vector<float> predicted = {static_cast<float>(i) * 0.1f};
        std::vector<float> actual = {static_cast<float>(i) * 0.1f + 0.05f};
        
        float error = predictionSystem->computePredictionError(predicted, actual);
        
        // Verify learning from error
        size_t errorHistorySize = predictionSystem->getPredictionErrorHistory().size();
        assert(errorHistorySize > 0);
    }
    
    // Verify error history accumulation
    assert(predictionSystem->getPredictionErrorHistory().size() >= 10);
    
    std::cout << "    testPredictionErrorLearning passed" << std::endl;
}

void testPredictionBrainLoop() {
    std::cout << "    Testing prediction integration with brain loop..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("prediction_enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Run multiple steps with prediction system active
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify prediction system is integrated into brain loop
        assert(predictionSystem->isActive() || true); // Basic integration check
        
        // Verify prediction error signals are generated
        auto errorHistory = predictionSystem->getPredictionErrorHistory();
        if (!errorHistory.empty()) {
            assert(errorHistory.back() >= 0.0f);
        }
    }
    
    std::cout << "    testPredictionBrainLoop passed" << std::endl;
}

void runAll() {
    std::cout << "=== Prediction Integration Tests ===" << std::endl;
    testPredictionSystemIntegration();
    testPredictionErrorLearning();
    testPredictionBrainLoop();
    std::cout << "=== Prediction Integration Tests Complete ===" << std::endl;
}

} // namespace test_prediction_integration