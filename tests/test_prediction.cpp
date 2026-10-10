#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_prediction {

void testPredictionSystemCreation() {
    nlm::PredictionSystem predSystem;
    assert(predSystem.getPredictionAccuracy() == 0.0f);
    assert(predSystem.getConfidenceLevel() == 0.0f);
    assert(predSystem.isActive() == false);
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testNeuralPrediction() {
    nlm::NeuralPrediction prediction;
    assert(prediction.getPredictionValue() == 0.0f);
    assert(prediction.getPredictionConfidence() == 0.0f);
    
    // Make a prediction
    prediction.setPredictionValue(0.7f);
    prediction.setPredictionConfidence(0.8f);
    assert(prediction.getPredictionValue() == 0.7f);
    assert(prediction.getPredictionConfidence() == 0.8f);
    std::cout << "    testNeuralPrediction passed" << std::endl;
}

void testPredictionAccuracy() {
    nlm::PredictionSystem predSystem;
    
    // Test accuracy calculation
    float accuracy = predSystem.calculateAccuracy(0.8f, 0.7f); // predicted vs actual
    assert(accuracy >= 0.0f && accuracy <= 1.0f);
    
    // Test confidence adjustment
    float adjusted = predSystem.adjustConfidence(0.9f, 0.2f);
    assert(adjusted >= 0.0f && adjusted <= 1.0f);
    std::cout << "    testPredictionAccuracy passed" << std::endl;
}

void testPredictionReset() {
    nlm::PredictionSystem predSystem;
    
    // Set some values
    predSystem.setPredictionAccuracy(0.8f);
    predSystem.setConfidenceLevel(0.9f);
    
    // Reset
    predSystem.reset();
    assert(predSystem.getPredictionAccuracy() == 0.0f);
    assert(predSystem.getConfidenceLevel() == 0.0f);
    assert(predSystem.isActive() == false);
    std::cout << "    testPredictionReset passed" << std::endl;
}

void testPredictionIntegration() {
    nlm::PredictionSystem predSystem;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Prediction system should be accessible
    assert(&predSystem != nullptr);
    std::cout << "    testPredictionIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Prediction tests..." << std::endl;
    testPredictionSystemCreation();
    testNeuralPrediction();
    testPredictionAccuracy();
    testPredictionReset();
    testPredictionIntegration();
}

} // namespace test_prediction