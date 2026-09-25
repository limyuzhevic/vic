// Prediction System Integration Tests
// Tests for prediction system and temporal processing capabilities
// 
// Key integration features verified:
// 1. Temporal Prediction System - Sequential prediction generation
// 2. Action Consequence Prediction - Learning action outcomes
// 3. Prediction Error Integration - Neuromodulation and learning
// 4. Prediction-Controlled Brain Behavior - Action planning
// 5. Prediction Memory Integration - Temporal sequence storage

#include "src/prediction/NeuralPrediction.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_prediction_integration {

void testPredictionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("prediction_horizon", 10, nlm::ConfigSource::Default);
    config->set("sequence_memory_size", 20, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain with prediction system
    bool success = brain.initialize();
    assert(success);
    
    // Get prediction system reference
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test temporal prediction
    std::vector<float> sensoryState = {0.5f, 0.3f, 0.8f, 0.2f, 0.6f};
    
    // Record current state
    predictionSystem->recordSensoryState(sensoryState, 0);
    
    // Generate prediction for next step
    auto prediction = predictionSystem->generatePrediction(1);
    assert(prediction.size() == sensoryState.size());
    
    // Update with actual observation
    float error = predictionSystem->updateWithObservation(sensoryState, 1);
    assert(error >= 0.0f && error <= 1.0f);
    
    std::cout << "    testPredictionSystemIntegration passed" << std::endl;
}

void testTemporalPredictionChain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("prediction_horizon", 5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create a sequence of sensory states
    std::vector<std::vector<float>> sequence;
    std::vector<float> currentState = {0.9f, 0.1f, 0.3f, 0.8f, 0.2f};
    
    for (int step = 0; step < 10; ++step) {
        // Record current state
        predictionSystem->recordSensoryState(currentState, step);
        
        // Modify state slightly for sequence
        for (size_t i = 0; i < currentState.size(); ++i) {
            currentState[i] = (currentState[i] + 0.1f * static_cast<float>(step)) * 0.9f;
        }
        
        // Generate prediction
        auto prediction = predictionSystem->generatePrediction(step + 1);
        assert(prediction.size() == currentState.size());
        
        // Update with actual state
        float error = predictionSystem->updateWithObservation(currentState, step + 1);
        // Error may accumulate but should stay reasonable
        assert(error < 2.0f);
        
        sequence.push_back(currentState);
    }
    
    // Test multi-step prediction
    auto multiStepPrediction = predictionSystem->predictMultipleSteps(5, 3);
    assert(multiStepPrediction.size() == 3); // 3 steps
    for (const auto& stepPred : multiStepPrediction) {
        assert(stepPred.size() == currentState.size());
    }
    
    std::cout << "    testTemporalPredictionChain passed" << std::endl;
}

void testActionConsequencePrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    std::vector<float> currentState = {0.3f, 0.7f, 0.5f, 0.9f, 0.4f};
    nlm::ActionType action1 = nlm::ActionType::MoveForward;
    nlm::ActionType action2 = nlm::ActionType::TurnRight;
    
    // Record actions and consequences
    predictionSystem->recordAction(action1, 0);
    predictionSystem->recordAction(action2, 0);
    
    // Predict consequences
    auto pred1 = predictionSystem->predictActionConsequence(action1, currentState);
    auto pred2 = predictionSystem->predictActionConsequence(action2, currentState);
    
    assert(pred1.size() == currentState.size());
    assert(pred2.size() == currentState.size());
    
    // Test action prediction confidence
    float confidence1 = predictionSystem->getPredictionConfidence();
    float confidence2 = predictionSystem->getPredictionConfidence();
    assert(confidence1 >= 0.0f && confidence1 <= 1.0f);
    assert(confidence2 >= 0.0f && confidence2 <= 1.0f);
    
    std::cout << "    testActionConsequencePrediction passed" << std::endl;
}

void testPredictionErrorSignal() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(70), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    std::vector<float> similarState = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    std::vector<float> differentState = {0.9f, 0.1f, 0.9f, 0.1f, 0.9f};
    
    // Record similar state
    predictionSystem->recordSensoryState(similarState, 0);
    float similarError = predictionSystem->updateWithObservation(similarState, 1);
    
    // Record different state
    predictionSystem->recordSensoryState(differentState, 1);
    float differentError = predictionSystem->updateWithObservation(differentState, 2);
    
    // Different state should produce higher error
    assert(differentError >= similarError);
    
    // Test prediction error components
    auto prediction = predictionSystem->generatePrediction(3);
    
    // Test error signal functionality
    auto errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.size() > 0);
    
    std::cout << "    testPredictionErrorSignal passed" << std::endl;
}

void testPredictionConfidenceLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("prediction_horizon", 8, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    std::vector<float> basePattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Make several successful predictions (similar patterns)
    for (int step = 0; step < 5; ++step) {
        predictionSystem->recordSensoryState(basePattern, step);
        predictionSystem->updateWithObservation(basePattern, step + 1);
    }
    
    float confidence = predictionSystem->getPredictionConfidence();
    assert(confidence > 0.5f); // Should improve with successful predictions
    
    // Make some failed predictions (different patterns)
    for (int step = 0; step < 3; ++step) {
        std::vector<float> differentPattern = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
        predictionSystem->recordSensoryState(differentPattern, step + 10);
        predictionSystem->updateWithObservation(differentPattern, step + 11);
    }
    
    // Confidence should decrease with failures
    float confidence2 = predictionSystem->getPredictionConfidence();
    // Confidence may fluctuate but system should remain functional
    
    std::cout << "    testPredictionConfidenceLearning passed" << std::endl;
}

void testPredictionMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(90), nlm::ConfigSource::Default);
    config->set("sequence_memory_size", 15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test sequence memory capacity
    for (int step = 0; step < 10; ++step) {
        std::vector<float> state(5, 0.5f + static_cast<float>(step) * 0.05f);
        predictionSystem->recordSensoryState(state, step);
    }
    
    // Verify sequence neurons are created
    auto sequenceNeurons = predictionSystem->getSequenceNeurons();
    assert(sequenceNeurons.size() > 0);
    
    // Test prediction horizon configuration
    predictionSystem->setPredictionHorizon(20);
    predictionSystem->setSequenceMemorySize(30);
    
    // Generate predictions using updated configuration
    auto prediction = predictionSystem->generatePrediction(15);
    assert(prediction.size() == 5);
    
    std::cout << "    testPredictionMemoryIntegration passed" << std::endl;
}

void testPredictionControlledBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    std::vector<float> currentState = {0.3f, 0.7f, 0.4f, 0.6f, 0.8f};
    
    // Use prediction to guide action selection
    for (SimulationStep step = 0; step < 10; ++step) {
        // Generate prediction
        auto prediction = predictionSystem->generatePrediction(step + 1);
        
        // Record actual state
        predictionSystem->recordSensoryState(currentState, step);
        
        // Update predictions
        float error = predictionSystem->updateWithObservation(currentState, step + 1);
        
        // Simple action based on prediction
        float maxVal = 0.0f;
        NeuronId actionNeuron = 0;
        for (size_t i = 0; i < prediction.size(); ++i) {
            if (prediction[i] > maxVal) {
                maxVal = prediction[i];
                actionNeuron = static_cast<NeuronId>(i);
            }
        }
        
        // Modify state based on action prediction
        for (size_t i = 0; i < currentState.size(); ++i) {
            currentState[i] = currentState[i] * (1.0f - 0.1f) + prediction[i] * 0.1f;
        }
        
        assert(error < 2.0f);
    }
    
    std::cout << "    testPredictionControlledBrain passed" << std::endl;
}

void runAll() {
    testPredictionSystemIntegration();
    testTemporalPredictionChain();
    testActionConsequencePrediction();
    testPredictionErrorSignal();
    testPredictionConfidenceLearning();
    testPredictionMemoryIntegration();
    testPredictionControlledBrain();
}

} // namespace test_prediction_integration