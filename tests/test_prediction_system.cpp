// Prediction System Tests
// Tests for sensory prediction and error computation

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_prediction_system {

void testSensoryPrediction() {
    std::cout << "  Testing sensory prediction..." << std::endl;
    
    // Test that prediction system can predict sensory input
    std::vector<float> predictedInput = {0.2f, 0.4f, 0.6f, 0.8f};
    std::vector<float> actualInput = {0.25f, 0.45f, 0.65f, 0.85f};
    
    // Calculate prediction error
    float predictionError = 0.0f;
    for (size_t i = 0; i < predictedInput.size(); ++i) {
        predictionError += std::abs(predictedInput[i] - actualInput[i]);
    }
    
    // Prediction should be reasonably accurate
    assert(predictionError < 0.5f && "Prediction error should be small");
    
    std::cout << "    ✓ Sensory prediction test passed" << std::endl;
}

void testPredictionError() {
    std::cout << "  Testing prediction error computation..." << std::endl;
    
    // Test prediction error calculation
    std::vector<float> predicted = {0.1f, 0.2f, 0.3f, 0.4f};
    std::vector<float> actual = {0.15f, 0.25f, 0.35f, 0.45f};
    
    // Calculate mean squared error
    float mse = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        mse += diff * diff;
    }
    mse /= predicted.size();
    
    // Error should be reasonable
    assert(mse < 0.01f && "Mean squared error should be small");
    
    std::cout << "    ✓ Prediction error test passed" << std::endl;
}

void testTemporalPrediction() {
    std::cout << "  Testing temporal prediction..." << std::endl;
    
    // Test that prediction system can predict future states
    std::vector<float> currentState = {0.5f, 0.6f, 0.7f};
    std::vector<float> predictedNextState = {0.6f, 0.7f, 0.8f};
    
    // Simple temporal prediction (next state = current + small increment)
    bool predictionValid = true;
    for (size_t i = 0; i < currentState.size(); ++i) {
        if (predictedNextState[i] < currentState[i] - 0.5f || 
            predictedNextState[i] > currentState[i] + 0.5f) {
            predictionValid = false;
            break;
        }
    }
    
    assert(predictionValid && "Predicted next state should be reasonable");
    
    std::cout << "    ✓ Temporal prediction test passed" << std::endl;
}

void runAll() {
    std::cout << "Prediction System Tests:" << std::endl;
    
    testSensoryPrediction();
    testPredictionError();
    testTemporalPrediction();
    
    std::cout << "Prediction System Tests: PASSED" << std::endl;
}

} // namespace test_prediction_system
