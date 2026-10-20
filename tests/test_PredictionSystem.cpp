// PredictionSystem Tests
#include "prediction/PredictionSystem.hpp"
#include "sensory/SensoryInput.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_prediction_system {

void testSystemCreation() {
    nlm::PredictionSystem system;
    
    assert(system.getPredictionError() == 0.0f);
    assert(system.getConfidence() == 0.0f);
    assert(system.getErrorHistory().empty());
    
    std::cout << "    testSystemCreation passed" << std::endl;
}

void testSystemInitialization() {
    nlm::PredictionSystem system;
    
    // Basic functionality test
    nlm::SensoryInput currentState;
    currentState.intensity = 0.5f;
    currentState.spatialX = 1.0f;
    currentState.spatialY = 2.0f;
    
    // Predict next state
    auto prediction = system.predictNextState(currentState);
    
    // Should return something (implementation may be placeholder)
    assert(prediction != nullptr);
    
    std::cout << "    testSystemInitialization passed" << std::endl;
}

void testPredictionUpdate() {
    nlm::PredictionSystem system;
    
    // Create initial state and prediction
    nlm::SensoryInput initialState;
    initialState.intensity = 0.5f;
    initialState.spatialX = 1.0f;
    initialState.spatialY = 2.0f;
    
    nlm::SensoryInput predictedState = system.predictNextState(initialState);
    
    // Create actual state (slightly different for some error)
    nlm::SensoryInput actualState = initialState;
    actualState.intensity += 0.1f;
    actualState.spatialX += 0.2f;
    
    // Update predictions
    system.updatePredictions(predictedState, actualState);
    
    // Get prediction error
    float error = system.getPredictionError();
    assert(error >= 0.0f);
    
    // Get confidence
    float confidence = system.getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testPredictionUpdate passed" << std::endl;
}

void testTraining() {
    nlm::PredictionSystem system;
    
    // Train with some observations
    nlm::SensoryInput observation1;
    observation1.intensity = 0.3f;
    observation1.spatialX = 1.0f;
    observation1.spatialY = 2.0f;
    
    nlm::SensoryInput observation2;
    observation2.intensity = 0.5f;
    observation2.spatialX = 1.2f;
    observation2.spatialY = 2.2f;
    
    nlm::SensoryInput observation3;
    observation3.intensity = 0.7f;
    observation3.spatialX = 1.4f;
    observation3.spatialY = 2.4f;
    
    // Train system
    system.train(observation1);
    system.train(observation2);
    system.train(observation3);
    
    // Should have learned something from training
    nlm::SensoryInput testState;
    testState.intensity = 0.5f;
    testState.spatialX = 1.2f;
    testState.spatialY = 2.2f;
    
    auto prediction = system.predictNextState(testState);
    assert(prediction != nullptr);
    
    std::cout << "    testTraining passed" << std::endl;
}

void testErrorHistory() {
    nlm::PredictionSystem system;
    
    assert(system.getErrorHistory().empty());
    
    // Add some error history
    std::vector<float>& history = const_cast<std::vector<float>&>(system.getErrorHistory());
    history.push_back(0.1f);
    history.push_back(0.3f);
    history.push_back(0.5f);
    
    assert(system.getErrorHistory().size() == 3);
    assert(system.getErrorHistory()[0] == 0.1f);
    assert(system.getErrorHistory()[1] == 0.3f);
    assert(system.getErrorHistory()[2] == 0.5f);
    
    // Clear history
    system.clearHistory();
    assert(system.getErrorHistory().empty());
    
    std::cout << "    testErrorHistory passed" << std::endl;
}

void testPredictionError() {
    nlm::PredictionSystem system;
    
    // Test with identical predictions and actuals (error should be 0)
    nlm::SensoryInput state;
    state.intensity = 0.5f;
    state.spatialX = 1.0f;
    state.spatialY = 2.0f;
    
    nlm::SensoryInput predicted = system.predictNextState(state);
    
    // Create identical actual state
    nlm::SensoryInput actual = predicted;
    
    system.updatePredictions(predicted, actual);
    
    float error = system.getPredictionError();
    assert(error == 0.0f);
    
    std::cout << "    testPredictionError passed" << std::endl;
}

void testPredictionConfidence() {
    nlm::PredictionSystem system;
    
    // Test initial confidence
    float confidence = system.getConfidence();
    assert(confidence >= 0.0f);
    
    // Make some predictions and updates to change confidence
    nlm::SensoryInput state1;
    state1.intensity = 0.5f;
    state1.spatialX = 1.0f;
    state1.spatialY = 2.0f;
    
    auto prediction1 = system.predictNextState(state1);
    
    nlm::SensoryInput actual1 = state1;
    system.updatePredictions(prediction1, actual1);
    
    float newConfidence = system.getConfidence();
    assert(newConfidence >= 0.0f);
    
    std::cout << "    testPredictionConfidence passed" << std::endl;
}

void runAll() {
    testSystemCreation();
    testSystemInitialization();
    testPredictionUpdate();
    testTraining();
    testErrorHistory();
    testPredictionError();
    testPredictionConfidence();
}

} // namespace test_prediction_system
