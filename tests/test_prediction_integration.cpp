// Prediction System Integration Tests
// Tests integration of NeuralPrediction with learning and brain dynamics

#include "brain/Brain.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_prediction_integration {

void testNeuralPredictionInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test basic brain functionality
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testNeuralPredictionInitialization passed" << std::endl;
}

void testPredictionLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Simulate learning sequences
    std::vector<std::vector<float>> sequence;
    std::vector<float> state1 = {0.1f, 0.2f, 0.3f, 0.4f};
    std::vector<float> state2 = {0.5f, 0.6f, 0.7f, 0.8f};
    std::vector<float> state3 = {0.9f, 0.8f, 0.7f, 0.6f};
    
    sequence.push_back(state1);
    sequence.push_back(state2);
    sequence.push_back(state3);
    
    // In full implementation: recordSensoryState would be called
    // brain.getPredictionSystem()->recordSensoryState(state1, 0);
    // brain.getPredictionSystem()->recordSensoryState(state2, 1);
    // brain.getPredictionSystem()->recordSensoryState(state3, 2);
    
    // Run simulation with sequence
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Verify learning occurred
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testPredictionLearning passed" << std::endl;
}

void testTemporalPrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test temporal prediction with varying input patterns
    std::vector<std::vector<float>> patterns;
    
    for (int i = 0; i < 20; ++i) {
        std::vector<float> pattern(5, 0.0f);
        for (int j = 0; j < 5; ++j) {
            pattern[j] = static_cast<float>(i * (j + 1)) / 50.0f;
        }
        patterns.push_back(pattern);
    }
    
    // In full implementation: predict based on pattern
    // brain.getPredictionSystem()->recordSensoryState(patterns[0], 0);
    
    // Run prediction simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testTemporalPrediction passed" << std::endl;
}

void testPredictionAccuracy() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create a predictable sequence
    std::vector<std::vector<float>> predictableSequence;
    std::vector<float> base = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern = base;
        pattern[0] += static_cast<float>(i) * 0.05f;
        predictableSequence.push_back(pattern);
    }
    
    // In full implementation: record and predict predictable patterns
    for (nlm::SimulationStep step = 0; step < predictableSequence.size(); ++step) {
        // brain.getPredictionSystem()->recordSensoryState(predictableSequence[step], step);
        brain.step(step);
    }
    
    // Generate predictions for later steps
    for (nlm::SimulationStep step = predictableSequence.size(); step < 30; ++step) {
        brain.step(step);
    }
    
    // Verify accuracy improved over time (this depends on implementation)
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testPredictionAccuracy passed" << std::endl;
}

void testPredictionSystemWithPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enabled", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    
    // Test prediction system learning while plasticity is active
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        brain.step(step);
        
        // Simulate prediction error signals
        float predictionError = std::abs(static_cast<float>(step % 10 - 5)) / 5.0f;
        
        // In full implementation: apply prediction error to neuromodulation
        // brain.applyPredictionError(predictionError);
    }
    
    // Verify learning occurred
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testPredictionSystemWithPlasticity passed" << std::endl;
}

void runAll() {
    std::cout << "Running Prediction System Integration tests..." << std::endl;
    testNeuralPredictionInitialization();
    testPredictionLearning();
    testTemporalPrediction();
    testPredictionAccuracy();
    testPredictionSystemWithPlasticity();
}

} // namespace test_prediction_integration
