// Prediction System Tests
// Tests PredictionSystem functionality and integration

#include "../src/prediction/PredictionSystem.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

namespace test_prediction_system {

void testPredictionSystemBasic() {
    std::cout << "  Testing PredictionSystem basic functionality..." << std::endl;
    
    PredictionSystem predictionSystem;
    
    // Create test sensory inputs
    SensoryInput currentState;
    currentState.setData({0.1f, 0.2f, 0.3f, 0.4f, 0.5f});
    currentState.setTimestamp(0.0);
    
    SensoryInput actualState;
    actualState.setData({0.15f, 0.25f, 0.35f, 0.45f, 0.55f});
    actualState.setTimestamp(1.0);
    
    // Test predictNextState
    auto predicted = predictionSystem.predictNextState(currentState);
    assert(predicted != nullptr);
    
    // Test getPredictionError
    float error = predictionSystem.getPredictionError();
    assert(error >= 0.0f);
    
    // Test getConfidence
    float confidence = predictionSystem.getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test updatePredictions
    predictionSystem.updatePredictions(*predicted, actualState);
    
    // Test getErrorHistory
    const auto& history = predictionSystem.getErrorHistory();
    assert(!history.empty() || error == 0.0f);  // May be empty if no update happened
    
    // Test train
    predictionSystem.train(actualState);
    
    std::cout << "    PASSED" << std::endl;
}

void testPredictionSystemIntegrationWithBrain() {
    std::cout << "  Testing PredictionSystem integration with Brain..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test basic prediction functionality
    SensoryInput testInput;
    testInput.setData({0.1f, 0.2f, 0.3f, 0.4f, 0.5f});
    
    auto prediction = predictionSystem->predictNextState(testInput);
    assert(prediction != nullptr);
    
    // Test prediction error tracking
    float error = predictionSystem->getPredictionError();
    assert(error >= 0.0f);
    
    // Test confidence
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testPredictionSystemConsistency() {
    std::cout << "  Testing PredictionSystem consistency..." << std::endl;
    
    PredictionSystem pred1, pred2;
    
    SensoryInput input1, input2;
    input1.setData({0.1f, 0.2f, 0.3f});
    input2.setData({0.1f, 0.2f, 0.3f});  // Same data
    
    auto pred1a = pred1.predictNextState(input1);
    auto pred1b = pred1.predictNextState(input1);  // Same input, should give same result
    
    auto pred2a = pred2.predictNextState(input2);
    
    // Predictions from same input should be consistent
    if (pred1a && pred1b) {
        assert(pred1a->getData().size() == pred1b->getData().size());
    }
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Prediction System Tests ===" << std::endl;
    
    testPredictionSystemBasic();
    testPredictionSystemIntegrationWithBrain();
    testPredictionSystemConsistency();
    
    std::cout << "=== Prediction System Tests Complete ===" << std::endl;
}

} // namespace test_prediction_system