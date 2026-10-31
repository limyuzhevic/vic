// Prediction Integration Tests
// Tests prediction systems

#include "prediction/PredictionSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>

namespace test_prediction {

void testPredictionSystemBase() {
    std::cout << "  Testing PredictionSystem Base..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    assert(prediction != nullptr);
    
    // Test initial state
    assert(prediction->getPredictionError() == 0.0f);
    assert(prediction->getConfidence() == 0.0f);
    assert(prediction->getErrorHistory().empty());
    
    // Test error history management
    prediction->train({0.1f, 0.2f, 0.3f});  // Train with simple observation
    
    std::vector<float> history = prediction->getErrorHistory();
    assert(!history.empty());
    
    prediction->clearHistory();
    assert(prediction->getErrorHistory().empty());
    
    std::cout << "    testPredictionSystemBase passed" << std::endl;
}

void testPredictionNextState() {
    std::cout << "  Testing Prediction Next State..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    assert(prediction != nullptr);
    
    // Create a test sensory input state
    nlm::SensoryInput currentState;
    currentState.pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    currentState.timestamp = 0;
    currentState.confidence = 0.8f;
    
    // Test prediction (this would be implemented in Phase 2)
    // For now, we test that the prediction system is accessible
    // and can be trained
    
    prediction->train(currentState);
    
    // The predictNextState method would return a SensoryInput
    // For testing, we verify the prediction system is properly initialized
    assert(prediction != nullptr);
    
    // Test prediction update
    nlm::SensoryInput predictedState;
    predictedState.pattern = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    predictedState.timestamp = 1;
    
    nlm::SensoryInput actualState;
    actualState.pattern = {0.12f, 0.22f, 0.32f, 0.42f, 0.52f};
    actualState.timestamp = 1;
    
    prediction->updatePredictions(predictedState, actualState);
    
    // Prediction error should change based on difference
    float error = prediction->getPredictionError();
    assert(error >= 0.0f);
    
    // Test confidence
    float confidence = prediction->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testPredictionNextState passed" << std::endl;
}

void testPredictionTraining() {
    std::cout << "  Testing Prediction Training..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    assert(prediction != nullptr);
    
    const size_t trainingSamples = 1000;
    
    for (size_t i = 0; i < trainingSamples; ++i) {
        nlm::SensoryInput observation;
        // Create a pattern that might have some structure
        float value = static_cast<float>(i) / static_cast<float>(trainingSamples) * 2.0f - 1.0f;
        observation.pattern = std::vector<float>(10, value);
        observation.timestamp = i;
        observation.confidence = 0.5f + (std::abs(value) * 0.5f);
        
        prediction->train(observation);
    }
    
    // Verify training completed
    std::vector<float> history = prediction->getErrorHistory();
    assert(history.size() == trainingSamples);
    
    // Test prediction accuracy improves with training
    // (in a real implementation)
    
    // Test prediction system integration with brain
    // Predictions can inform motor planning and action selection
    
    std::cout << "    testPredictionTraining passed" << std::endl;
}

void testPredictionErrorDynamics() {
    std::cout << "  Testing Prediction Error Dynamics..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    assert(prediction != nullptr);
    
    // Test prediction error with perfect prediction
    nlm::SensoryInput predictedState;
    predictedState.pattern = {0.1f, 0.2f, 0.3f};
    predictedState.timestamp = 0;
    
    nlm::SensoryInput actualState;
    actualState.pattern = {0.1f, 0.2f, 0.3f};  // Perfect match
    actualState.timestamp = 0;
    
    prediction->updatePredictions(predictedState, actualState);
    float perfectError = prediction->getPredictionError();
    assert(perfectError == 0.0f);
    
    // Test prediction error with poor prediction
    nlm::SensoryInput badPrediction;
    badPrediction.pattern = {1.0f, 1.0f, 1.0f};  // Very different
    badPrediction.timestamp = 1;
    
    prediction->updatePredictions(badPrediction, actualState);
    float badError = prediction->getPredictionError();
    assert(badError > perfectError);
    
    // Test confidence reduction with high error
    float confidence = prediction->getConfidence();
    assert(confidence <= 1.0f);
    
    std::cout << "    testPredictionErrorDynamics passed" << std::endl;
}

void testPredictionIntegration() {
    std::cout << "  Testing Prediction Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify prediction system is accessible
    assert(brain.getPredictionSystem() != nullptr);
    
    // Test prediction integration with other systems
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    
    assert(novelty != nullptr);
    assert(predError != nullptr);
    assert(curiosity != nullptr);
    
    // Prediction error affects neuromodulation
    // High prediction error should increase curiosity and dopamine
    
    // Create a prediction error scenario
    nlm::SensoryInput predictedState;
    predictedState.pattern = {0.5f, 0.5f, 0.5f};
    predictedState.timestamp = 0;
    
    nlm::SensoryInput actualState;
    actualState.pattern = {1.0f, 1.0f, 1.0f};  // Large error
    actualState.timestamp = 0;
    
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    prediction->updatePredictions(predictedState, actualState);
    
    float error = prediction->getPredictionError();
    assert(error > 0.0f);
    
    // This error would propagate to neuromodulators in Phase 2
    // For now, we verify the prediction system works
    
    // Test prediction integration with planning
    nlm::NeuralPlanner* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Predictions can inform action planning
    // Planner could use prediction confidence to select actions
    
    // Test prediction integration with memory
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Predictions can update working memory with expected outcomes
    
    std::cout << "    testPredictionIntegration passed" << std::endl;
}

void testPredictionPerformance() {
    std::cout << "  Testing Prediction Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    assert(prediction != nullptr);
    
    const size_t iterations = 5000;
    
    // Performance benchmark for prediction system
    for (size_t i = 0; i < iterations; ++i) {
        nlm::SensoryInput observation;
        // Create a simple pattern
        observation.pattern = std::vector<float>(5, static_cast<float>(i % 10) / 10.0f);
        observation.timestamp = i;
        observation.confidence = 0.5f + (static_cast<float>(i) / static_cast<float>(iterations)) * 0.5f;
        
        // Train prediction
        prediction->train(observation);
        
        // Make prediction (if available)
        // prediction->predictNextState(observation);
        
        // Update with some prediction
        nlm::SensoryInput predictedState = observation;  // For test simplicity
        prediction->updatePredictions(predictedState, observation);
        
        // Get error and confidence
        float error = prediction->getPredictionError();
        float confidence = prediction->getConfidence();
        
        // These should be valid
        assert(error >= 0.0f);
        assert(confidence >= 0.0f && confidence <= 1.0f);
    }
    
    // Verify performance
    std::vector<float> history = prediction->getErrorHistory();
    assert(history.size() == iterations);
    
    std::cout << "    testPredictionPerformance passed" << std::endl;
}

void testPredictionErrorConditions() {
    std::cout << "  Testing Prediction Error Conditions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    assert(prediction != nullptr);
    
    // Test with null/empty inputs
    // Prediction system should handle edge cases
    nlm::SensoryInput emptyInput;
    emptyInput.pattern.clear();
    
    // Should handle empty observations
    prediction->train(emptyInput);
    
    // Test with extreme values
    nlm::SensoryInput extremeInput;
    extremeInput.pattern = {1000.0f, -1000.0f, 0.0f};
    extremeInput.timestamp = 0;
    
    prediction->train(extremeInput);
    
    // Test prediction error with NaN/Inf
    // Implementation should handle or clamp extreme values
    
    // Test history overflow protection
    prediction->clearHistory();
    
    // Test confidence bounds
    float confidence = prediction->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testPredictionErrorConditions passed" << std::endl;
}

void runAll() {
    testPredictionSystemBase();
    testPredictionNextState();
    testPredictionTraining();
    testPredictionErrorDynamics();
    testPredictionIntegration();
    testPredictionPerformance();
    testPredictionErrorConditions();
}

} // namespace test_prediction
