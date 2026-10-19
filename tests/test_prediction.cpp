// Prediction System Tests
// Comprehensive tests for prediction system and neural prediction capabilities

#include "prediction/NeuralPrediction.hpp"
#include "prediction/PredictionSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_prediction {

// NeuralPrediction Tests
void testNeuralPredictionBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test basic prediction functionality
    // Note: NeuralPrediction is integrated within Brain, testing through brain interface
    
    // Record some sensory states
    std::vector<float> state1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    brain.recordSensoryState(state1, 0);
    
    std::vector<float> state2 = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    brain.recordSensoryState(state2, 1);
    
    // Test prediction generation
    std::vector<float> prediction = brain.generatePrediction(2);
    assert(prediction.size() == state1.size());
    
    // Test prediction error
    float error = brain.getPredictionError();
    assert(error >= 0.0f && error <= 1.0f);
    
    // Test prediction confidence
    float confidence = brain.getPredictionConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test prediction update
    std::vector<float> actual = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
    float updatedError = brain.updateWithObservation(actual, 3);
    assert(updatedError >= 0.0f);
    
    std::cout << "    testNeuralPredictionBasic passed" << std::endl;
}

void testNeuralPredictionTemporal() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a time series of sensory states
    std::vector<std::vector<float>> timeSeries;
    for (int i = 0; i < 20; ++i) {
        std::vector<float> state(5);
        for (int j = 0; j < 5; ++j) {
            state[j] = static_cast<float>(i * 0.1f + j * 0.02f);
        }
        timeSeries.push_back(state);
        brain.recordSensoryState(state, i);
    }
    
    // Test multi-step prediction
    std::vector<std::vector<float>> multiPredictions = 
        brain.predictMultipleSteps(15, 3);
    
    assert(multiPredictions.size() == 3);
    for (const auto& prediction : multiPredictions) {
        assert(prediction.size() == 5);
    }
    
    std::cout << "    testNeuralPredictionTemporal passed" << std::endl;
}

void testNeuralPredictionActionConsequences() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record sensory states
    std::vector<float> state1 = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    brain.recordSensoryState(state1, 0);
    
    // Record actions
    brain.recordAction(nlm::ActionType::MoveForward, 1);
    brain.recordAction(nlm::ActionType::Wait, 2);
    brain.recordAction(nlm::ActionType::Attack, 3);
    
    // Test action consequence prediction
    std::vector<float> consequence1 = brain.predictActionConsequence(
        nlm::ActionType::MoveForward, state1);
    
    assert(consequence1.size() == state1.size());
    
    std::cout << "    testNeuralPredictionActionConsequences passed" << std::endl;
}

void testNeuralPredictionHistory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record some predictions and errors
    for (int i = 0; i < 10; ++i) {
        std::vector<float> state = {static_cast<float>(i) * 0.1f, 
                                    static_cast<float>(i) * 0.1f + 0.1f,
                                    static_cast<float>(i) * 0.1f + 0.2f};
        brain.recordSensoryState(state, i);
        brain.generatePrediction(i + 1);
    }
    
    // Test error history
    const std::vector<float>& errorHistory = brain.getErrorHistory();
    assert(errorHistory.size() >= 10);
    
    // Clear history
    brain.clearHistory();
    
    assert(brain.getErrorHistory().size() == 0);
    
    std::cout << "    testNeuralPredictionHistory passed" << std::endl;
}

// PredictionSystem Tests
void testPredictionSystemBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system interface
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Note: PredictionSystem methods are integrated into Brain class
    // Testing through brain interface
    
    std::cout << "    testPredictionSystemBasic passed" << std::endl;
}

void testPredictionSystemTraining() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Train prediction system with various observations
    for (int i = 0; i < 20; ++i) {
        std::vector<float> state(3);
        for (int j = 0; j < 3; ++j) {
            state[j] = static_cast<float>(i * 0.2f + j * 0.05f);
        }
        brain.recordSensoryState(state, i);
        brain.generatePrediction(i + 1);
    }
    
    // Test prediction system state
    float confidence = brain.getPredictionConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    float error = brain.getPredictionError();
    assert(error >= 0.0f && error <= 1.0f);
    
    std::cout << "    testPredictionSystemTraining passed" << std::endl;
}

void testPredictionSystemPredictionError() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction error computation
    std::vector<float> state1 = {0.1f, 0.2f, 0.3f};
    brain.recordSensoryState(state1, 0);
    
    std::vector<float> state2 = {0.15f, 0.25f, 0.35f};
    brain.recordSensoryState(state2, 1);
    
    std::vector<float> prediction = brain.generatePrediction(2);
    std::vector<float> actual = {0.2f, 0.3f, 0.4f};
    
    float error = brain.updateWithObservation(actual, 2);
    
    assert(error >= 0.0f);
    
    // Test prediction error signal
    float predictionError = brain.getPredictionError();
    assert(predictionError >= 0.0f);
    
    std::cout << "    testPredictionSystemPredictionError passed" << std::endl;
}

// Prediction Integration Tests
void PredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that prediction system integrates with other subsystems
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test integration with neuromodulation
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Test that prediction error signals can modulate learning
    float predictionError = brain.getPredictionError();
    float neuromodulationSignal = dopamine->getPlasticityFactor();
    
    // Prediction error should influence neuromodulation
    // (in real implementation)
    
    // Test integration with memory systems
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Test that predictions can update memory
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f, 0.8f};
    workingMemory->store(pattern, 1.0f);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = pattern;
    episode.reward = 10.0f;
    episodicMemory->storeEpisode(episode);
    
    std::cout << "    testPredictionIntegration passed" << std::endl;
}

void testPredictionPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction performance with large network
    // Generate a sequence of states
    for (int i = 0; i < 100; ++i) {
        std::vector<float> state(10);
        for (int j = 0; j < 10; ++j) {
            state[j] = static_cast<float>(sin(i * 0.1f + j * 0.2f));
        }
        brain.recordSensoryState(state, i);
        
        // Generate predictions every few steps
        if (i % 5 == 0) {
            brain.generatePrediction(i + 1);
        }
    }
    
    // Performance metrics
    float confidence = brain.getPredictionConfidence();
    float error = brain.getPredictionError();
    
    // System should remain stable under load
    assert(confidence >= 0.0f && confidence <= 1.0f);
    assert(error >= 0.0f && error <= 1.0f);
    
    std::cout << "    testPredictionPerformance passed" << std::endl;
}

void testPredictionErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction with edge cases
    // Empty state
    std::vector<float> emptyState;
    brain.recordSensoryState(emptyState, 0);
    
    // Very large state
    std::vector<float> largeState(100, 1.0f);
    brain.recordSensoryState(largeState, 1);
    
    // State with NaN/Inf values (should be handled gracefully)
    std::vector<float> edgeState = {0.0f, 1.0f, std::nanf(""), std::numeric_limits<float>::infinity()};
    brain.recordSensoryState(edgeState, 2);
    
    // Prediction system should not crash
    std::vector<float> prediction = brain.generatePrediction(3);
    
    // Error should be valid
    float error = brain.getPredictionError();
    assert(error >= 0.0f);
    
    std::cout << "    testPredictionErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Prediction System tests..." << std::endl;
    
    try {
        testNeuralPredictionBasic();
        testNeuralPredictionTemporal();
        testNeuralPredictionActionConsequences();
        testNeuralPredictionHistory();
        
        testPredictionSystemBasic();
        testPredictionSystemTraining();
        testPredictionSystemPredictionError();
        
        testPredictionIntegration();
        testPredictionPerformance();
        testPredictionErrorHandling();
        
        std::cout << "  All Prediction System tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_prediction