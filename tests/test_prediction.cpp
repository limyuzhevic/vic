// Prediction System Tests
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_prediction {

void testPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("prediction_confidence_threshold", 0.5f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system functionality
    float predictionAccuracy = predictionSystem->getAccuracy();
    assert(predictionAccuracy >= 0.0f && predictionAccuracy <= 1.0f);
    
    std::cout << "    testPredictionSystem passed" << std::endl;
}

void testNeuralPrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* neuralPrediction = brain.getPredictionSystem();
    if (neuralPrediction) {
        // Test neural prediction functionality
        float prediction = neuralPrediction->makePrediction();
        assert(prediction >= 0.0f);
        
        std::cout << "    testNeuralPrediction passed" << std::endl;
    }
}

void runAll() {
    testPredictionSystem();
    testNeuralPrediction();
}

} // namespace test_prediction
