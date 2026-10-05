// Prediction System Integration Tests
// Phase 3.3: Integration tests for prediction system

#include "brain/Brain.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "prediction/PredictionSystem.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>

namespace test_prediction_integration {

void testPredictionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("prediction_sensitivity", 0.4, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system integration
    brain.getPredictionError(); // Should work without crashing
    
    brain.step(0);
    
    assert(brain.getTotalNeuronCount() == 200);
    assert(std::isfinite(brain.getAverageFiringRate()));
    
    std::cout << "    testPredictionSystemIntegration passed" << std::endl;
}

void testNeuralPredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("prediction_accuracy", 0.85, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neural prediction integration
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        // The brain should have prediction capabilities
        brain.step(step);
        
        assert(brain.getTotalNeuronCount() == 180);
        assert(brain.getPredictionError() >= 0.0f);
    }
    
    std::cout << "    testNeuralPredictionIntegration passed" << std::endl;
}

void testPredictionAccuracyIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(220), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("prediction_confidence", 0.9, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction accuracy integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        // The brain should handle prediction errors
        brain.step(step);
        
        assert(brain.getTotalNeuronCount() == 220);
        assert(brain.getPredictionError() >= 0.0f);
        assert(brain.getPredictionError() <= 1.0f); // Normalized to 0-1
    }
    
    std::cout << "    testPredictionAccuracyIntegration passed" << std::endl;
}

void testCompletePredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("prediction_sensitivity", 0.6, nlm::ConfigSource::Default);
    config->set("prediction_accuracy", 0.8, nlm::ConfigSource::Default);
    config->set("prediction_confidence", 0.95, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Complete prediction integration test
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Prediction system should work with other systems
        brain.step(step);
        
        // Verify prediction integration
        assert(brain.getTotalNeuronCount() == 250);
        assert(std::isfinite(brain.getAverageFiringRate()));
        assert(brain.getPredictionError() >= 0.0f);
    }
    
    std::cout << "    testCompletePredictionIntegration passed" << std::endl;
}

void testPredictionPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("prediction_sensitivity", 0.7, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark prediction performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalPredictionEffect = 0.0f;
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        // Prediction system updates
        brain.step(step);
        
        totalPredictionEffect += brain.getPredictionError();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    testPredictionPerformance: " << duration.count() << " microseconds for 60 prediction steps" << std::endl;
    std::cout << "    Average prediction effect: " << (totalPredictionEffect / 60.0f) << std::endl;
    
    std::cout << "    testPredictionPerformance passed" << std::endl;
}

void testPredictionResourceManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(280), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("prediction_sensitivity", 0.65, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction resource management under load
    for (int i = 0; i < 5; ++i) {
        // Heavy prediction usage
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.step(step);
        }
        
        // Reset and verify cleanup
        brain.reset();
        assert(brain.initialize());
    }
    
    std::cout << "    testPredictionResourceManagement passed" << std::endl;
}

void testPredictionSystemRobustness() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(320), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("prediction_sensitivity", 0.75, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system robustness
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        // Variable prediction inputs
        brain.step(step);
        
        // Stress prediction system
        assert(brain.getTotalNeuronCount() == 320);
        assert(std::isfinite(brain.getAverageFiringRate()));
        assert(brain.getPredictionError() >= 0.0f);
    }
    
    std::cout << "    testPredictionSystemRobustness passed" << std::endl;
}

void testPredictionIntegrationWithOtherSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("prediction_sensitivity", 0.8, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction integration with other systems
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Prediction should work with reward modulation
        brain.applyRewardModulation(0.4f, 0.3f);
        
        // Prediction should work with development
        brain.updateDevelopment(step);
        
        // Step the integrated system
        brain.step(step);
        
        // Verify all systems are working together
        assert(brain.getTotalNeuronCount() == 350);
        assert(std::isfinite(brain.getAverageFiringRate()));
        assert(brain.getPredictionError() >= 0.0f);
    }
    
    std::cout << "    testPredictionIntegrationWithOtherSystems passed" << std::endl;
}

void runAll() {
    testPredictionSystemIntegration();
    testNeuralPredictionIntegration();
    testPredictionAccuracyIntegration();
    testCompletePredictionIntegration();
    testPredictionPerformance();
    testPredictionResourceManagement();
    testPredictionSystemRobustness();
    testPredictionIntegrationWithOtherSystems();
}

} // namespace test_prediction_integration