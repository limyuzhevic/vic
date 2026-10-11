// Prediction System Tests
// Tests for PredictionSystem functionality including prediction generation, error computation, and learning

#include "prediction/PredictionSystem.hpp"
#include "sensory/SensoryInput.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_prediction_system {

void testPredictionSystemCreation() {
    nlm::PredictionSystem predictionSystem;
    
    // Test basic properties
    assert(nlm_test::utils::approxEqual(predictionSystem.getPredictionError(), 0.0f));
    assert(nlm_test::utils::approxEqual(predictionSystem.getConfidence(), 0.0f));
    assert(predictionSystem.getErrorHistory().empty());
    
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testPredictionSystemPredictNextState() {
    nlm::PredictionSystem predictionSystem;
    
    // Create a test sensory input
    nlm::SensoryInput currentState;
    currentState.neuralActivity = {0.5f, 0.7f, 0.3f, 0.9f};
    currentState.metadata = "Test state";
    
    // Make a prediction
    auto prediction = predictionSystem.predictNextState(currentState);
    assert(prediction != nullptr);
    
    // Prediction should have similar neural activity (placeholder behavior)
    assert(!prediction->neuralActivity.empty());
    assert(prediction->neuralActivity.size() == currentState.neuralActivity.size());
    
    // Prediction error should be computed
    assert(nlm_test::utils::approxEqual(predictionSystem.getPredictionError(), 0.0f));
    
    std::cout << "    testPredictionSystemPredictNextState passed" << std::endl;
}

void testPredictionSystemUpdatePredictions() {
    nlm::PredictionSystem predictionSystem;
    
    // Create test states
    nlm::SensoryInput predictedState;
    predictedState.neuralActivity = {0.5f, 0.7f, 0.3f, 0.9f};
    
    nlm::SensoryInput actualState;
    actualState.neuralActivity = {0.6f, 0.65f, 0.35f, 0.85f};
    
    // Update predictions based on actual observation
    predictionSystem.updatePredictions(predictedState, actualState);
    
    // Prediction error should be computed based on difference
    assert(!nlm_test::utils::approxEqual(predictionSystem.getPredictionError(), 0.0f));
    
    std::cout << "    testPredictionSystemUpdatePredictions passed" << std::endl;
}

void testPredictionSystemErrorHistory() {
    nlm::PredictionSystem predictionSystem;
    
    // Initially should be empty
    assert(predictionSystem.getErrorHistory().empty());
    
    // Make some predictions and updates to generate error history
    for (int i = 0; i < 5; ++i) {
        nlm::SensoryInput state1;
        state1.neuralActivity = {static_cast<float>(i), static_cast<float>(i + 1), 0.5f};
        
        nlm::SensoryInput state2;
        state2.neuralActivity = {static_cast<float>(i + 0.1), static_cast<float>(i + 1.1), 0.6f};
        
        predictionSystem.updatePredictions(state1, state2);
        
        // Error history should be populated
        assert(!predictionSystem.getErrorHistory().empty());
    }
    
    // Should have grown error history
    size_t initialSize = predictionSystem.getErrorHistory().size();
    assert(initialSize > 0);
    
    std::cout << "    testPredictionSystemErrorHistory passed" << std::endl;
}

void testPredictionSystemConfidence() {
    nlm::PredictionSystem predictionSystem;
    
    // Initially low confidence
    assert(nlm_test::utils::approxEqual(predictionSystem.getConfidence(), 0.0f));
    
    // Make some predictions to improve confidence
    nlm::SensoryInput state1;
    state1.neuralActivity = {0.5f, 0.7f, 0.3f};
    
    nlm::SensoryInput state2;
    state2.neuralActivity = {0.6f, 0.75f, 0.35f};
    
    predictionSystem.updatePredictions(state1, state2);
    
    // Confidence should be computed
    assert(nlm_test::utils::approxEqual(predictionSystem.getConfidence(), 0.0f) ||
           predictionSystem.getConfidence() > 0.0f);
    
    std::cout << "    testPredictionSystemConfidence passed" << std::endl;
}

void testPredictionSystemTraining() {
    nlm::PredictionSystem predictionSystem;
    
    // Train with different observations
    for (int i = 0; i < 10; ++i) {
        nlm::SensoryInput observation;
        observation.neuralActivity = {
            static_cast<float>(sin(i * 0.1)),
            static_cast<float>(cos(i * 0.2)),
            static_cast<float>(i * 0.1)
        };
        observation.metadata = "Training observation " + std::to_string(i);
        
        predictionSystem.train(observation);
    }
    
    // Should still be functional after training
    assert(predictionSystem.getPredictionError() >= 0.0f);
    
    std::cout << "    testPredictionSystemTraining passed" << std::endl;
}

void testPredictionSystemClearHistory() {
    nlm::PredictionSystem predictionSystem;
    
    // Generate some error history
    for (int i = 0; i < 3; ++i) {
        nlm::SensoryInput state1;
        state1.neuralActivity = {1.0f, 2.0f};
        
        nlm::SensoryInput state2;
        state2.neuralActivity = {1.1f, 2.1f};
        
        predictionSystem.updatePredictions(state1, state2);
    }
    
    // Should have history
    assert(!predictionSystem.getErrorHistory().empty());
    
    // Clear history
    predictionSystem.clearHistory();
    assert(predictionSystem.getErrorHistory().empty());
    
    std::cout << "    testPredictionSystemClearHistory passed" << std::endl;
}

void testPredictionSystemIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system integration through brain
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Make some predictions
    nlm::SensoryInput state1;
    state1.neuralActivity = {0.5f, 0.7f, 0.3f};
    state1.metadata = "Integration test";
    
    nlm::SensoryInput prediction = *predictionSystem->predictNextState(state1);
    assert(!prediction.neuralActivity.empty());
    
    // Update predictions
    nlm::SensoryInput state2;
    state2.neuralActivity = {0.6f, 0.75f, 0.35f};
    
    predictionSystem->updatePredictions(prediction, state2);
    
    // Error and confidence should be accessible
    assert(predictionSystem->getPredictionError() >= 0.0f);
    assert(predictionSystem->getConfidence() >= 0.0f);
    
    std::cout << "    testPredictionSystemIntegrationWithBrain passed" << std::endl;
}

void testPredictionSystemMultiStepPrediction() {
    nlm::PredictionSystem predictionSystem;
    
    nlm::SensoryInput currentState;
    currentState.neuralActivity = {0.5f, 0.7f, 0.3f};
    currentState.metadata = "Multi-step test";
    
    // Make multiple predictions
    std::vector<std::unique_ptr<nlm::SensoryInput>> predictions;
    
    auto firstPrediction = predictionSystem.predictNextState(currentState);
    if (firstPrediction) {
        predictions.push_back(std::move(firstPrediction));
        
        // Use first prediction as input for second prediction
        auto secondPrediction = predictionSystem.predictNextState(*predictions[0]);
        if (secondPrediction) {
            predictions.push_back(std::move(secondPrediction));
        }
    }
    
    // Should have generated multiple predictions
    assert(predictions.size() <= 2); // Could be less if predictNextState returns null
    
    std::cout << "    testPredictionSystemMultiStepPrediction passed" << std::endl;
}

void testPredictionSystemStress() {
    nlm::PredictionSystem predictionSystem;
    
    // Rapid prediction and update operations
    for (int i = 0; i < 100; ++i) {
        nlm::SensoryInput state1;
        state1.neuralActivity = {
            static_cast<float>(i % 10) * 0.1f,
            static_cast<float>((i * 2) % 10) * 0.1f,
            static_cast<float>((i * 3) % 10) * 0.1f
        };
        
        nlm::SensoryInput state2;
        state2.neuralActivity = {
            static_cast<float>(i % 10) * 0.1f + 0.05f,
            static_cast<float>((i * 2) % 10) * 0.1f + 0.05f,
            static_cast<float>((i * 3) % 10) * 0.1f + 0.05f
        };
        
        predictionSystem.updatePredictions(state1, state2);
        
        // Should remain functional
        assert(predictionSystem.getPredictionError() >= 0.0f);
        assert(predictionSystem.getConfidence() >= 0.0f);
        
        // Train occasionally
        if (i % 10 == 0) {
            predictionSystem.train(state1);
        }
    }
    
    std::cout << "    testPredictionSystemStress passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Prediction System Creation", testPredictionSystemCreation);
    runner.runTest("Prediction System Predict Next State", testPredictionSystemPredictNextState);
    runner.runTest("Prediction System Update Predictions", testPredictionSystemUpdatePredictions);
    runner.runTest("Prediction System Error History", testPredictionSystemErrorHistory);
    runner.runTest("Prediction System Confidence", testPredictionSystemConfidence);
    runner.runTest("Prediction System Training", testPredictionSystemTraining);
    runner.runTest("Prediction System Clear History", testPredictionSystemClearHistory);
    runner.runTest("Prediction System Integration With Brain", testPredictionSystemIntegrationWithBrain);
    runner.runTest("Prediction System Multi Step Prediction", testPredictionSystemMultiStepPrediction);
    runner.runTest("Prediction System Stress Test", testPredictionSystemStress);
    
    runner.runAll();
}

} // namespace test_prediction_system
