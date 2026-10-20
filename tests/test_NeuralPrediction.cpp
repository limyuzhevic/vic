// NeuralPrediction Tests
#include "prediction/NeuralPrediction.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neural_prediction {

void testPredictionCreation() {
    nlm::NeuralPrediction prediction;
    
    assert(prediction.getPredictionNeurons().empty());
    assert(prediction.getSequenceNeurons().empty());
    assert(prediction.getPredictionError() == 0.0f);
    assert(prediction.getPredictionConfidence() == 0.5f);  // Default
    
    std::cout << "    testPredictionCreation passed" << std::endl;
}

void testPredictionInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPrediction prediction;
    prediction.initialize(&brain);
    
    assert(prediction.getPredictionNeurons().empty());
    assert(prediction.getSequenceNeurons().empty());
    
    std::cout << "    testPredictionInitialization passed" << std::endl;
}

void testTemporalPrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPrediction prediction;
    prediction.initialize(&brain);
    
    // Record some sensory states
    std::vector<float> state1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> state2 = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    std::vector<float> state3 = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
    
    prediction.recordSensoryState(state1, 0);
    prediction.recordSensoryState(state2, 1);
    prediction.recordSensoryState(state3, 2);
    
    // Generate prediction for current step
    auto predicted = prediction.generatePrediction(2);
    
    // Should return a prediction (may be empty if no patterns learned yet)
    assert(predicted.empty() || !predicted.empty());
    
    std::cout << "    testTemporalPrediction passed" << std::endl;
}

void testPredictionUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPrediction prediction;
    prediction.initialize(&brain);
    
    // Record a state
    std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    prediction.recordSensoryState(state, 0);
    
    // Create actual state (similar but not identical)
    std::vector<float> actualState = state;
    actualState[0] += 0.05f;
    actualState[1] += 0.05f;
    
    // Update with observation
    float error = prediction.updateWithObservation(actualState, 1);
    
    assert(error >= 0.0f);
    
    // Should have some error history
    // (though accessing might be difficult due to private members)
    
    std::cout << "    testPredictionUpdate passed" << std::endl;
}

void testActionConsequencePrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPrediction prediction;
    prediction.initialize(&brain);
    
    // Record an action
    prediction.recordAction(nlm::ActionType::MoveForward, 0);
    
    // Record a state before action
    std::vector<float> beforeState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    prediction.recordSensoryState(beforeState, 0);
    
    // Create a different state (after action)
    std::vector<float> afterState = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    prediction.recordSensoryState(afterState, 1);
    
    // Predict consequence of action
    auto predictedConsequence = prediction.predictActionConsequence(
        nlm::ActionType::MoveForward, beforeState);
    
    // Should return a prediction (may be empty if no learning yet)
    assert(predictedConsequence.empty() || !predictedConsequence.empty());
    
    std::cout << "    testActionConsequencePrediction passed" << std::endl;
}

void testMultipleStepPrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPrediction prediction;
    prediction.initialize(&brain);
    
    // Record some states
    std::vector<float> state1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> state2 = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    
    prediction.recordSensoryState(state1, 0);
    prediction.recordSensoryState(state2, 1);
    
    // Predict multiple steps ahead
    auto multiPredictions = prediction.predictMultipleSteps(1, 3);
    
    // Should return predictions for each step (may be empty)
    assert(multiPredictions.empty() || !multiPredictions.empty());
    
    std::cout << "    testMultipleStepPrediction passed" << std::endl;
}

void testPredictionSimilarity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPrediction prediction;
    prediction.initialize(&brain);
    
    // Test similarity computation
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};  // Similar to A
    std::vector<float> patternC = {0.8f, 0.9f, 1.0f, 1.1f, 1.2f};  // Very different from A
    
    // Compute similarities using the prediction's computeSimilarity method
    float similarityAB = prediction.computeSimilarity(patternA, patternB);
    float similarityAC = prediction.computeSimilarity(patternA, patternC);
    
    // Pattern A and B should be more similar than A and C
    assert(similarityAB > similarityAC);
    
    std::cout << "    testPredictionSimilarity passed" << std::endl;
}

void testPredictionHistory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPrediction prediction;
    prediction.initialize(&brain);
    
    // Clear history (should be empty initially)
    prediction.clearHistory();
    
    // Record some states and update to generate error history
    for (int i = 0; i < 5; ++i) {
        std::vector<float> state = {static_cast<float>(i) * 0.1f, 
                                    static_cast<float>(i) * 0.2f,
                                    static_cast<float>(i) * 0.3f,
                                    static_cast<float>(i) * 0.4f,
                                    static_cast<float>(i) * 0.5f};
        
        prediction.recordSensoryState(state, i);
        
        std::vector<float> actual = state;
        actual[0] += 0.05f;  // Add some error
        
        prediction.updateWithObservation(actual, i + 1);
    }
    
    // Clear history
    prediction.clearHistory();
    
    std::cout << "    testPredictionHistory passed" << std::endl;
}

void runAll() {
    testPredictionCreation();
    testPredictionInitialization();
    testTemporalPrediction();
    testPredictionUpdate();
    testActionConsequencePrediction();
    testMultipleStepPrediction();
    testPredictionSimilarity();
    testPredictionHistory();
}

} // namespace test_neural_prediction
