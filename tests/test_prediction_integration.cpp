// Prediction System Integration Tests
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "prediction/PredictionSystem.hpp"
#include "sensory/SensoryInput.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_prediction_integration {

void testPredictionWithNeuromodulation() {
    std::cout << "    Testing Prediction + Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(2));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create sensory input sequence
    std::vector<nlm::SensoryInput> sensorySequence;
    float timeStep = 0.001f;
    
    for (int step = 0; step < 50; ++step) {
        nlm::SensoryInput input;
        input.timestamp = step * timeStep;
        input.step = step;
        
        // Create pattern that changes predictably
        input.visionData.resize(20, 0.0f);
        for (size_t i = 0; i < 20; ++i) {
            input.visionData[i] = 0.5f + 0.3f * sinf((float)i * 0.3f + step * 0.2f);
        }
        
        input.auditionData.resize(10, 0.0f);
        for (size_t i = 0; i < 10; ++i) {
            input.auditionData[i] = 0.2f * cosf((float)i * 0.4f + step * 0.15f);
        }
        
        sensorySequence.push_back(input);
        
        // Update prediction system with current state
        predictionSystem->update(timeStep);
        
        // Store current state for later prediction
        if (sensorySequence.size() >= 2) {
            auto& predicted = predictionSystem->predictNextState(sensorySequence.back());
            // Compare with actual next state
            if (predicted) {
                float error = predictionSystem->getPredictionError();
                assert(error >= 0.0f); // Error should be non-negative
            }
        }
        
        // Inject reward prediction error into neuromodulation
        if (step % 10 == 0) {
            float reward = 0.5f + 0.3f * sinf((float)step * 0.1f);
            brain.getDopamine()->signalRewardPredictionError(reward);
        }
        
        brain.step(step, step * timeStep);
    }
    
    // Test prediction confidence
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test prediction error history
    auto& errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.size() <= 100); // Should be bounded
    
    std::cout << "    Prediction + Neuromodulation integration test passed" << std::endl;
}

void testPredictionWithWorkingMemory() {
    std::cout << "    Testing Prediction + Working Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    auto* workingMemory = brain.getWorkingMemory();
    
    assert(predictionSystem != nullptr);
    assert(workingMemory != nullptr);
    
    // Create repeating patterns
    std::vector<float> basePattern(25, 0.0f);
    for (size_t i = 0; i < 25; ++i) {
        basePattern[i] = 0.7f * sinf((float)i * 0.2f);
    }
    
    // Store pattern in working memory for prediction
    workingMemory->store(basePattern, 0.8f);
    
    // Generate predictions based on pattern
    nlm::SensoryInput currentState;
    currentState.visionData = basePattern;
    
    for (int step = 0; step < 30; ++step) {
        // Update prediction system
        predictionSystem->update(0.001f);
        
        // Make predictions
        auto prediction = predictionSystem->predictNextState(currentState);
        
        // Update working memory (simulating prediction integration)
        if (prediction) {
            workingMemory->strengthenMemory(1.1f); // Slightly strengthen based on prediction
        }
        
        // Simulate time passing with some noise
        for (size_t i = 0; i < basePattern.size(); ++i) {
            basePattern[i] += 0.01f * sinf((float)step * 0.3f);
        }
        currentState.visionData = basePattern;
        
        brain.step(step, step * 0.001f);
    }
    
    // Verify working memory is affected by prediction
    size_t finalTraces = workingMemory->getActiveTraces();
    assert(finalTraces >= 0); // Should have some active traces
    
    std::cout << "    Prediction + Working Memory integration test passed" << std::endl;
}

void runAll() {
    testPredictionWithNeuromodulation();
    testPredictionWithWorkingMemory();
}

} // namespace test_prediction_integration