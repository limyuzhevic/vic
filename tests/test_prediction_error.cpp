// PredictionError Tests
#include "neuromodulation/PredictionError.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_prediction_error {

void testPredictionErrorCreation() {
    nlm::PredictionError predictionError;
    
    assert(predictionError.getError() == 0.0f);
    assert(predictionError.getMagnitude() == 0.0f);
    assert(predictionError.getHistory().empty());
    
    std::cout << "    testPredictionErrorCreation passed" << std::endl;
}

void testPredictionErrorInitialization() {
    nlm::PredictionError predictionError;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    predictionError.initialize(&brain);
    
    assert(predictionError.getError() == 0.0f);
    
    std::cout << "    testPredictionErrorInitialization passed" << std::endl;
}

void testPredictionErrorComputation() {
    nlm::PredictionError predictionError;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    predictionError.initialize(&brain);
    
    // Test zero prediction error
    predictionError.computeError(0.0f, 0.0f);
    assert(predictionError.getError() == 0.0f);
    assert(predictionError.getMagnitude() == 0.0f);
    
    // Test positive prediction error (actual > predicted)
    predictionError.computeError(0.5f, 1.0f);
    assert(predictionError.getError() == 0.5f);
    assert(predictionError.getMagnitude() == 0.5f);
    
    // Test negative prediction error (actual < predicted)
    predictionError.computeError(1.0f, 0.5f);
    assert(predictionError.getError() == -0.5f);
    assert(predictionError.getMagnitude() == 0.5f);
    
    // Check that error was added to history
    const auto& history = predictionError.getHistory();
    assert(!history.empty());
    assert(history.size() == 2); // We computed twice
    
    std::cout << "    testPredictionErrorComputation passed" << std::endl;
}

void testPredictionErrorHistory() {
    nlm::PredictionError predictionError;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    predictionError.initialize(&brain);
    
    // Add several errors to history
    predictionError.computeError(0.0f, 1.0f); // error = 1.0
    predictionError.computeError(0.5f, 1.0f); // error = 0.5
    predictionError.computeError(1.0f, 0.5f); // error = -0.5
    
    const auto& history = predictionError.getHistory();
    assert(history.size() == 3);
    assert(history[0] == 1.0f);
    assert(history[1] == 0.5f);
    assert(history[2] == -0.5f);
    
    // Test clearing history
    predictionError.clearHistory();
    assert(predictionError.getHistory().empty());
    
    std::cout << "    testPredictionErrorHistory passed" << std::endl;
}

void testPredictionErrorUpdatePrediction() {
    nlm::PredictionError predictionError;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    predictionError.initialize(&brain);
    
    predictionError.computeError(0.5f, 1.0f);
    assert(predictionError.getError() == 0.5f);
    
    predictionError.updatePrediction(0.8f);
    
    // Note: updatePrediction doesn't directly change error, just updates predictedValue
    // For now, just test that it doesn't crash
    
    std::cout << "    testPredictionErrorUpdatePrediction passed" << std::endl;
}

void testPredictionErrorIntegration() {
    // Test integration with other systems
    nlm::PredictionError predictionError;
    
    // PredictionError should be able to integrate with brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    predictionError.initialize(&brain);
    
    // Test basic functionality
    assert(predictionError.getError() == 0.0f);
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running PredictionError tests..." << std::endl;
    testPredictionErrorCreation();
    testPredictionErrorInitialization();
    testPredictionErrorComputation();
    testPredictionErrorHistory();
    testPredictionErrorUpdatePrediction();
    testPredictionErrorIntegration();
}

} // namespace test_prediction_error
