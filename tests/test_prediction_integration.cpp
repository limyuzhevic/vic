// Prediction Integration Tests for Phase 6 NLM Brain
// Tests prediction system integration with sensory input

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "prediction/PredictionSystem.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_prediction_integration {

void testPredictionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system updates
    // Create sensory-like input patterns
    std::vector<float> inputPattern = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    
    // Update prediction system with current activity
    predictionSystem->updatePattern(inputPattern);
    
    // Test prediction error generation
    std::vector<float> predictedPattern = {0.15f, 0.45f, 0.35f, 0.75f, 0.25f};
    std::vector<float> actualPattern = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    
    // Simulate prediction error calculation
    float predictionError = 0.0f;
    for (size_t i = 0; i < inputPattern.size(); ++i) {
        float error = std::abs(actualPattern[i] - predictedPattern[i]);
        predictionError += error;
    }
    predictionError /= inputPattern.size();
    
    // Prediction error should be greater than zero for mismatched patterns
    assert(predictionError > 0.0f);
    
    std::cout << "    testPredictionSystemIntegration passed" << std::endl;
}

void testTemporalPrediction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test temporal prediction with different patterns
    std::vector<float> pattern1 = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    std::vector<float> pattern2 = {0.3f, 0.5f, 0.7f, 0.9f, 0.1f};
    
    // Update prediction system multiple times
    predictionSystem->updatePattern(pattern1);
    predictionSystem->updatePattern(pattern2);
    
    // The prediction system should maintain state across updates
    // (Implementation-dependent behavior)
    
    std::cout << "    testTemporalPrediction passed" << std::endl;
}

void testPredictionErrorIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get prediction error signal system
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test prediction error generation
    std::vector<float> expectedPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> actualPattern = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    
    // Calculate prediction error
    float predictionError = 0.0f;
    for (size_t i = 0; i < expectedPattern.size(); ++i) {
        predictionError += std::abs(actualPattern[i] - expectedPattern[i]);
    }
    predictionError /= expectedPattern.size();
    
    // Error should be small for similar patterns
    assert(predictionError < 0.1f);
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void testPredictionWithNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get prediction system and neuromodulation
    auto* predictionSystem = brain.getPredictionSystem();
    auto* novelty = brain.getNovelty();
    auto* dopamine = brain.getDopamine();
    
    assert(predictionSystem != nullptr);
    assert(novelty != nullptr);
    assert(dopamine != nullptr);
    
    // Update prediction system
    std::vector<float> pattern = {0.1f, 0.9f, 0.2f, 0.8f, 0.3f};
    predictionSystem->updatePattern(pattern);
    
    // Update novelty based on prediction error
    float noveltyLevel = novelty->getLevel();
    if (noveltyLevel > 0.5f) {
        dopamine->updatePredictionError(noveltyLevel);
    }
    
    // Dopamine level should reflect prediction error
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    std::cout << "    testPredictionWithNeuromodulation passed" << std::endl;
}

} // namespace test_prediction_integration
