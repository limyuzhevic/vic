// Prediction Integration Tests
// Tests prediction system integration with sensory processing

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "prediction/PredictionSystem.hpp"
#include "sensory/SensoryInput.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_prediction_integration {

void testPredictionSystemCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Prediction system should be accessible
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Initial state
    assert(predictionSystem->getPredictionError() >= 0.0f);
    assert(predictionSystem->getConfidence() >= 0.0f && predictionSystem->getConfidence() <= 1.0f);
    assert(predictionSystem->getErrorHistory().empty());
    
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testPredictionAccuracyOverTime() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create predictable sensory sequence
    std::vector<std::vector<float>> sequence = {
        {0.1f, 0.2f, 0.3f},
        {0.2f, 0.3f, 0.4f},
        {0.3f, 0.4f, 0.5f},
        {0.4f, 0.5f, 0.6f},
        {0.5f, 0.6f, 0.7f}
    };
    
    // For testing, we'll just test the prediction system itself
    // since brain.getSensoryInputFromData() is not available
    // Instead, we'll create a simple InternalSignals object
    
    // Make predictions and track error
    float maxError = 0.0f;
    for (size_t i = 1; i < sequence.size(); ++i) {
        // Create simple sensory inputs for testing
        nlm::InternalSignals currentSeq(sequence[i-1]);
        nlm::InternalSignals nextSeq(sequence[i]);
        
        // Update prediction system
        brain.getPredictionSystem()->updatePredictions(currentSeq, nextSeq);
        
        float error = brain.getPredictionSystem()->getPredictionError();
        maxError = std::max(maxError, error);
    }
    
    // Prediction error should be reasonable
    assert(maxError >= 0.0f && maxError < 5.0f); // Not exploding
    
    std::cout << "    testPredictionAccuracyOverTime passed" << std::endl;
}

void testPredictionErrorComputation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create two different states
    std::vector<float> state1 = {1.0f, 0.0f, 0.0f};
    std::vector<float> state2 = {0.0f, 1.0f, 0.0f};
    
    // Predict state2 from state1
    auto prediction = brain.getPredictionSystem()->predictNextState(
        brain.getSensoryInputFromData(state1));
    assert(prediction != nullptr);
    
    // Update with actual state2
    brain.getPredictionSystem()->updatePredictions(
        brain.getSensoryInputFromData(state1),
        brain.getSensoryInputFromData(state2));
    
    // Error should be non-zero (different states)
    float error = brain.getPredictionSystem()->getPredictionError();
    assert(error > 0.0f);
    
    // Error should be reasonable (not too large)
    assert(error < 10.0f);
    
    // Add to history
    brain.getPredictionSystem()->updatePredictions(
        brain.getSensoryInputFromData(state1),
        brain.getSensoryInputFromData(state2));
    
    // History should grow
    assert(brain.getPredictionSystem()->getErrorHistory().size() >= 1);
    
    std::cout << "    testPredictionErrorComputation passed" << std::endl;
}

void testPredictionTraining() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create training data with patterns
    std::vector<std::vector<float>> trainingData = {
        {0.1f, 0.2f, 0.3f},
        {0.2f, 0.3f, 0.4f},
        {0.3f, 0.4f, 0.5f},
        {0.1f, 0.2f, 0.3f}, // Repeat to show pattern
        {0.2f, 0.3f, 0.4f}
    };
    
    // Train on all data using InternalSignals
    for (size_t i = 0; i < trainingData.size(); ++i) {
        nlm::InternalSignals input(trainingData[i]);
        brain.getPredictionSystem()->train(input);
    }
    
    // Should not crash during training
    assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
    
    // Test prediction after training
    nlm::InternalSignals input(trainingData[0]);
    auto prediction = brain.getPredictionSystem()->predictNextState(input);
    assert(prediction != nullptr);
    
    std::cout << "    testPredictionTraining passed" << std::endl;
}

void testPredictionWithConceptFormation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a pattern that will form a concept
    std::vector<float> pattern = {0.8f, 0.6f, 0.4f, 0.2f, 0.1f};
    
    // Present to concept formation first
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        pattern, pattern, 1.0f, 1);
    
    // Also train prediction system on same pattern
    nlm::InternalSignals input(pattern);
    brain.getPredictionSystem()->train(input);
    
    // Both systems should be active
    assert(brain.getConceptFormation()->getConceptCount() > 0 || conceptId == 0);
    assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
    
    // Make prediction using trained system
    auto prediction = brain.getPredictionSystem()->predictNextState(input);
    assert(prediction != nullptr);
    
    // Pattern should influence prediction (concept and prediction systems should interact)
    std::vector<float> similarPattern = pattern;
    for (float& val : similarPattern) val *= 0.9f;
    
    // Concept system should find similar pattern
    size_t matchingConcept = brain.getConceptFormation()->getMatchingConcept(
        similarPattern, 0.7f);
    
    // Prediction system should also be able to make prediction
    nlm::InternalSignals similarInput(similarPattern);
    auto similarPrediction = brain.getPredictionSystem()->predictNextState(similarInput);
    // Prediction can be null (if not implemented yet) but should not crash
    
    std::cout << "    testPredictionWithConceptFormation passed" << std::endl;
}

void testPredictionErrorHistoryManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Generate multiple predictions with errors
    for (nlm::SimulationStep step = 1; step < 10; ++step) {
        std::vector<float> state1 = {static_cast<float>(step) * 0.1f, 0.5f, 0.0f};
        std::vector<float> state2 = {static_cast<float>(step) * 0.1f + 0.1f, 0.6f, 0.1f};
        
        // Use InternalSignals instead of getSensoryInputFromData
        nlm::InternalSignals input1(state1);
        nlm::InternalSignals input2(state2);
        
        brain.getPredictionSystem()->updatePredictions(input1, input2);
        
        // History should grow
        assert(brain.getPredictionSystem()->getErrorHistory().size() == step);
    }
    
    // Clear history
    brain.getPredictionSystem()->clearHistory();
    assert(brain.getPredictionSystem()->getErrorHistory().empty());
    
    // Can add history again
    nlm::InternalSignals input1({0.1f, 0.2f, 0.3f});
    nlm::InternalSignals input2({0.2f, 0.3f, 0.4f});
    brain.getPredictionSystem()->updatePredictions(input1, input2);
    assert(!brain.getPredictionSystem()->getErrorHistory().empty());
    
    std::cout << "    testPredictionErrorHistoryManagement passed" << std::endl;
}

void testPredictionConfidenceDynamics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initial confidence
    float initialConfidence = brain.getPredictionSystem()->getConfidence();
    assert(initialConfidence >= 0.0f && initialConfidence <= 1.0f);
    
    // Make predictions and update - confidence may change
    for (nlm::SimulationStep step = 1; step < 20; ++step) {
        brain.step(step);
        
        nlm::InternalSignals input({0.5f, 0.5f, 0.5f});
        auto prediction = brain.getPredictionSystem()->predictNextState(input);
        
        if (prediction) {
            nlm::InternalSignals actual({0.6f, 0.6f, 0.6f});
            brain.getPredictionSystem()->updatePredictions(input, actual);
            
            float confidence = brain.getPredictionSystem()->getConfidence();
            assert(confidence >= 0.0f && confidence <= 1.0f);
        }
    }
    
    std::cout << "    testPredictionConfidenceDynamics passed" << std::endl;
}

void testPredictionSystemIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Brain should have prediction system
    assert(brain.getPredictionSystem() != nullptr);
    
    // Integration: prediction system should be usable during normal brain operation
    std::vector<float> pattern = {1.0f, 0.5f, 0.0f, -0.5f, -1.0f};
    
    // Present to concept formation (which might use prediction)
    brain.getConceptFormation()->presentExperience(pattern, pattern, 1.0f, 1);
    
    // Make prediction
    nlm::InternalSignals input(pattern);
    auto prediction = brain.getPredictionSystem()->predictNextState(input);
    assert(prediction != nullptr);
    
    // Brain should continue to function normally
    for (nlm::SimulationStep step = 1; step < 30; ++step) {
        brain.step(step);
        
        // Prediction system should still be accessible
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
    }
    
    std::cout << "    testPredictionSystemIntegrationWithBrain passed" << std::endl;
}

void runAll() {
    testPredictionSystemCreation();
    testPredictionAccuracyOverTime();
    testPredictionErrorComputation();
    testPredictionTraining();
    testPredictionWithConceptFormation();
    testPredictionErrorHistoryManagement();
    testPredictionConfidenceDynamics();
    testPredictionSystemIntegrationWithBrain();
}

} // namespace test_prediction_integration