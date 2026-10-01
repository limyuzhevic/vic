// Prediction System Tests
// Tests for NeuralPrediction and PredictionSystem

#include "prediction/NeuralPrediction.hpp"
#include "prediction/PredictionSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_prediction {

void testNeuralPrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction error signal
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test setting and getting prediction error
    predictionError->setPredictionError(0.5f);
    float error = predictionError->getPredictionError();
    assert(error == 0.5f);
    
    std::cout << "    testNeuralPrediction passed" << std::endl;
}

void testPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system update
    predictionSystem->updatePrediction(0.0f);
    
    // Test prediction system reset
    predictionSystem->reset();
    
    // Test prediction error history
    auto* predictionError = brain.getPredictionErrorSignal();
    predictionError->recordError(0.1f);
    predictionError->recordError(0.2f);
    
    auto errorHistory = predictionError->getErrorHistory();
    assert(errorHistory.size() >= 2);
    
    std::cout << "    testPredictionSystem passed" << std::endl;
}

void runAll() {
    testNeuralPrediction();
    testPredictionSystem();
}

} // namespace test_prediction