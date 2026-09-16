// Prediction System Tests
// Tests prediction mechanisms and reward prediction integration

#include "prediction/NeuralPrediction.hpp"
#include "prediction/NeuralPredictionSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_prediction_system {

void testPredictionGeneration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPrediction predictor;
    predictor.initialize(&brain);
    predictor.enableTemporalPrediction(true);
    
    // Record sensory state
    std::vector<float> sensoryState{0.5f, 0.7f, 0.9f, 0.3f, 0.6f};
    predictor.recordSensoryState(sensoryState, 0);
    
    // Generate prediction
    auto prediction = predictor.generatePrediction(1);
    assert(prediction.size() == sensoryState.size());
    
    // Update with actual observation
    float error = predictor.updateWithObservation(sensoryState, 1);
    assert(error >= 0.0f && error <= 1.0f);
    
    // Verify prediction confidence
    float confidence = predictor.getPredictionConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testPredictionGeneration passed" << std::endl;
}

void testRewardPrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPrediction predictor;
    predictor.initialize(&brain);
    predictor.enableTemporalPrediction(true);
    
    // Test reward prediction with sequence
    std::vector<float> states{
        0.2f, 0.3f, 0.4f, 0.5f, 0.6f,
        0.7f, 0.8f, 0.9f, 1.0f, 0.9f,
        0.8f, 0.7f, 0.6f, 0.5f, 0.4f
    };
    
    for (size_t i = 0; i < states.size() - 1; ++i) {
        predictor.recordSensoryState({states[i]}, i);
    }
    
    // Generate multi-step prediction
    auto multiStep = predictor.predictMultipleSteps(10, 3);
    assert(multiStep.size() == 3);
    for (const auto& step : multiStep) {
        assert(step.size() == 1);
        assert(step[0] >= 0.0f && step[0] <= 1.0f);
    }
    
    std::cout << "    testRewardPrediction passed" << std::endl;
}

void testActionConsequenceLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ActionConsequencePredictor actionPredictor;
    actionPredictor.initialize(&brain);
    
    // Record action-consequence mappings
    std::vector<float> state{0.5f, 0.3f, 0.7f};
    std::vector<float> consequence{0.8f, 0.6f, 0.9f};
    
    for (int action = 0; action < 6; ++action) {
        actionPredictor.recordExperience(
            static_cast<nlm::ActionType>(action),
            state,
            consequence,
            0.5f + (action * 0.1f)
        );
    }
    
    // Predict consequences
    auto predictedConsequence = actionPredictor.predictConsequence(
        nlm::ActionType::MoveForward, state
    );
    assert(predictedConsequence.size() == consequence.size());
    
    // Update with actual consequence
    actionPredictor.updatePrediction(
        nlm::ActionType::MoveForward,
        predictedConsequence,
        consequence,
        0.8f
    );
    
    // Verify confidence
    float confidence = actionPredictor.getConsequenceConfidence(
        nlm::ActionType::MoveForward, state
    );
    assert(confidence > 0.0f);
    
    std::cout << "    testActionConsequenceLearning passed" << std::endl;
}

void testPredictionErrorIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPrediction predictor;
    predictor.initialize(&brain);
    predictor.enableTemporalPrediction(true);
    
    // Create prediction errors
    std::vector<float> state{0.5f, 0.5f, 0.5f};
    predictor.recordSensoryState(state, 0);
    
    // Generate incorrect prediction
    auto wrongPrediction = predictor.generatePrediction(1);
    for (auto& val : wrongPrediction) {
        val = 1.0f - val;  // Inverse to create error
    }
    
    // Update with wrong prediction to create high error
    float error = predictor.updateWithObservation(state, 1);
    assert(error > 0.5f); // High error expected
    
    // Verify error history
    const auto& errorHistory = predictor.getErrorHistory();
    assert(!errorHistory.empty());
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void runAll() {
    testPredictionGeneration();
    testRewardPrediction();
    testActionConsequenceLearning();
    testPredictionErrorIntegration();
}

} // namespace test_prediction_system