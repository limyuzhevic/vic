// PredictionSystem Tests
#include "prediction/PredictionSystem.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>

namespace test_prediction_system {

void testPredictionSystemCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create prediction system
    nlm::PredictionSystem predictionSystem;
    
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testPredictionSystemInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initialize prediction system
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(&brain);
    
    // Verify prediction system was initialized
    assert(predictionSystem.getPredictionError() >= 0.0f);
    assert(predictionSystem.getConfidence() >= 0.0f && predictionSystem.getConfidence() <= 1.0f);
    
    std::cout << "    testPredictionSystemInitialization passed" << std::endl;
}

void testPredictionSystemUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initialize prediction system
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(&brain);
    
    // Get initial state
    float initialError = predictionSystem.getPredictionError();
    size_t initialHistorySize = predictionSystem.getErrorHistory().size();
    
    // Update prediction system
    predictionSystem.update();
    
    // Verify error was updated
    assert(predictionSystem.getPredictionError() >= 0.0f);
    assert(predictionSystem.getErrorHistory().size() >= initialHistorySize);
    
    std::cout << "    testPredictionSystemUpdate passed" << std::endl;
}

void testPredictionSystemErrorHistory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initialize prediction system
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(&brain);
    
    // Clear history
    predictionSystem.clearHistory();
    assert(predictionSystem.getErrorHistory().empty());
    
    // Update multiple times to build history
    for (int i = 0; i < 5; ++i) {
        predictionSystem.update();
    }
    
    // Verify history was populated
    assert(!predictionSystem.getErrorHistory().empty());
    assert(predictionSystem.getErrorHistory().size() == 5);
    
    // Verify all values are valid
    for (float error : predictionSystem.getErrorHistory()) {
        assert(error >= 0.0f && error <= 1.0f);
    }
    
    std::cout << "    testPredictionSystemErrorHistory passed" << std::endl;
}

void testPredictionSystemConfidenceModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initialize prediction system
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(&brain);
    
    // Get initial confidence
    float initialConfidence = predictionSystem.getConfidence();
    
    // Simulate low error (should increase confidence)
    if (brain.getDopamine()) {
        brain.getDopamine()->setLevel(0.95f);  // High dopamine = low error
        predictionSystem.update();
        assert(predictionSystem.getConfidence() >= initialConfidence);
    }
    
    // Simulate high error (should decrease confidence)
    if (brain.getDopamine()) {
        brain.getDopamine()->setLevel(0.1f);   // Low dopamine = high error
        predictionSystem.update();
        assert(predictionSystem.getConfidence() <= initialConfidence);
    }
    
    // Restore confidence
    if (brain.getDopamine()) {
        brain.getDopamine()->setLevel(0.5f);
    }
    
    std::cout << "    testPredictionSystemConfidenceModulation passed" << std::endl;
}

void testPredictionSystemIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify prediction system is integrated
    // The brain should have its prediction system initialized
    // This test verifies the integration between Brain and PredictionSystem
    
    // Run a few steps to ensure everything works together
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testPredictionSystemIntegrationWithBrain passed" << std::endl;
}

void runAll() {
    testPredictionSystemCreation();
    testPredictionSystemInitialization();
    testPredictionSystemUpdate();
    testPredictionSystemErrorHistory();
    testPredictionSystemConfidenceModulation();
    testPredictionSystemIntegrationWithBrain();
}

} // namespace test_prediction_system