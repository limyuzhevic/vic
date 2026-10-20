// Prediction System Tests
// Tests for prediction systems, neural prediction, etc.

#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_prediction {

void testPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::PredictionSystem predSystem(config);
    
    // Test basic operations
    assert(predSystem.getHorizon() > 0);
    assert(predSystem.getPredictionAccuracy() >= 0.0f && predSystem.getPredictionAccuracy() <= 1.0f);
    assert(predSystem.getErrorTolerance() >= 0.0f);
    
    // Test prediction
    std::vector<float> current_state{0.5f, 0.3f, 0.8f};
    std::vector<float> prediction = predSystem.predict(current_state);
    
    assert(prediction.size() > 0);
    
    std::cout << "    testPredictionSystem passed" << std::endl;
}

void testNeuralPrediction() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::NeuralPrediction neuralPred(config);
    
    // Test basic operations
    assert(neuralPred.getPredictionWeight() >= 0.0f && neuralPred.getPredictionWeight() <= 1.0f);
    assert(neuralPred.getPredictionError() >= 0.0f);
    
    // Test neural prediction
    std::vector<float> input{0.1f, 0.9f, 0.5f};
    std::vector<float> output = neuralPred.predict(input);
    
    assert(output.size() > 0);
    
    std::cout << "    testNeuralPrediction passed" << std::endl;
}

void runAll() {
    testPredictionSystem();
    testNeuralPrediction();
}

} // namespace test_prediction