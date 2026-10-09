// Test for Prediction System Integration
// Covers requirement: 2. Prediction System Tests

#include "prediction/PredictionSystem.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_prediction_system {

void testPredictionSystemCreation() {
    nlm::PredictionSystem prediction;
    
    // Test initial state
    assert(prediction.getPredictionCount() == 0);
    assert(prediction.getConfidenceThreshold() == 0.5f);
    
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testPredictionInputOutput() {
    nlm::PredictionSystem prediction;
    
    // Create test neuron
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set neuron state
    neuron.setMembranePotential(-65.0f);
    neuron.injectCurrent(2.5f);
    
    // Make a prediction
    float prediction = prediction.predict(neuron);
    assert(std::isfinite(prediction));
    
    // Update prediction with error
    prediction.update(neuron, 0.8f); // prediction value
    
    assert(prediction.getPredictionCount() == 1);
    
    std::cout << "    testPredictionInputOutput passed" << std::endl;
}

void testPredictionErrorHandling() {
    nlm::PredictionSystem prediction;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Make prediction for neuron with zero current
    float prediction = prediction.predict(neuron);
    
    // Update with different values
    prediction.update(neuron, 1.0f);
    
    // Test error tracking
    float predictionError = prediction.getPredictionError(neuron);
    assert(std::isfinite(predictionError));
    
    std::cout << "    testPredictionErrorHandling passed" << std::endl;
}

void testPredictionConfidence() {
    nlm::PredictionSystem prediction;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Make multiple predictions to build confidence
    for (int i = 0; i < 5; ++i) {
        prediction.update(neuron, 0.5f + (i * 0.1f));
    }
    
    // Check confidence
    bool confident = prediction.isConfident(neuron, 0.8f);
    assert(!confident); // Initial confidence should be low with few predictions
    
    std::cout << "    testPredictionConfidence passed" << std::endl;
}

void runAll() {
    testPredictionSystemCreation();
    testPredictionInputOutput();
    testPredictionErrorHandling();
    testPredictionConfidence();
}

} // namespace test_prediction_system
