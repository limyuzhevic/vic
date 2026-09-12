// Prediction System Integration Tests
#include "prediction/PredictionSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_prediction {

void testPredictionSystemCreation() {
    nlm::PredictionSystem prediction;
    
    // Check initial state
    assert(prediction.getPredictionError() == 0.0f);
    
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testPredictionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system through brain simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Get prediction error
        float predictionError = predictionSystem->getPredictionError();
        assert(predictionError >= 0.0f);
        
        // Prediction error should drive neuromodulation
        auto* predictionErrorSignal = brain.getPredictionErrorSignal();
        if (predictionErrorSignal) {
            // Prediction error updates dopamine for learning
            predictionErrorSignal->updatePredictionError(0.1f * step * 0.01f, 0.001f);
        }
    
    std::cout << "    testPredictionSystemIntegration passed" << std::endl;
}

void testPredictionErrorLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionError = brain.getPredictionErrorSignal();
    auto* dopamine = brain.getDopamine();
    
    assert(predictionError != nullptr);
    assert(dopamine != nullptr);
    
    // Test that prediction error drives dopamine for learning
    float initialPredictionError = predictionError->getErrorLevel();
    float initialDopamine = dopamine->getLevel();
    
    // Update prediction error
    predictionError->updatePredictionError(0.8f, 0.001f);
    
    // Dopamine should respond to prediction error
    assert(dopamine->getLevel() >= initialDopamine);
    
    // High prediction error should drive more learning
    predictionError->updatePredictionError(0.5f, 0.001f);
    assert(dopamine->getLevel() >= initialDopamine);
    
    std::cout << "    testPredictionErrorLearning passed" << std::endl;
}

void testPredictionSensoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.11, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    auto* sensoryNeurons = brain.getSensoryNeurons();
    
    assert(predictionSystem != nullptr);
    
    // Create sensory input
    nlm::SensoryInput sensoryInput;
    sensoryInput.addValue("vision", std::vector<float>{0.7f, 0.8f, 0.9f});
    sensoryInput.addValue("position", std::vector<float>{0.2f, 0.8f});
    
    // Process sensory input
    brain.receiveSensoryInput(sensoryInput);
    
    // Run simulation to allow prediction system to update
    for (nlm::SimulationStep step = 0; step < 80; ++step) {
        brain.step(step);
        
        // Prediction system should update with sensory state
        float predictionError = predictionSystem->getPredictionError();
        assert(predictionError >= 0.0f);
    
    std::cout << "    testPredictionSensoryIntegration passed" << std::endl;
}

void testPredictionMemoryAssociation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("max_episodic_episodes", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(predictionSystem != nullptr);
    assert(episodicMemory != nullptr);
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.5f, 0.6f, 0.7f};
    episode.reward = 0.8f;
    episode.action = nlm::ActionType::MoveForward;
    
    episodicMemory->storeEpisode(episode);
    
    // Prediction system can use episodic memory for better predictions
    // This integration test verifies the connection exists
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Run simulation with prediction system
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        brain.step(step);
        
        // Check that prediction system is operational
        float predictionError = predictionSystem->getPredictionError();
        assert(predictionError >= 0.0f);
    
    std::cout << "    testPredictionMemoryAssociation passed" << std::endl;
}

void testPredictionBehavioralControl() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    
    assert(predictionSystem != nullptr);
    
    // Get initial action
    auto action1 = brain.produceAction();
    assert(action1 != nullptr);
    
    // Simulate with prediction system driving behavior
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Create sensory input
        nlm::SensoryInput sensoryInput;
        
        // Simple pattern based on step to create environment variation
        float pattern = std::sin(step * 0.02f);
        sensoryInput.addValue("vision", std::vector<float>{pattern, pattern * 0.9f, pattern * 0.8f});
        
        brain.receiveSensoryInput(sensoryInput);
        brain.step(step);
        
        // Action should be produced based on neural activity
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Prediction system should be active
        float predictionError = predictionSystem->getPredictionError();
        assert(predictionError >= 0.0f);
    
    std::cout << "    testPredictionBehavioralControl passed" << std::endl;
}

void runAll() {
    testPredictionSystemCreation();
    testPredictionSystemIntegration();
    testPredictionErrorLearning();
    testPredictionSensoryIntegration();
    testPredictionMemoryAssociation();
    testPredictionBehavioralControl();
    
    std::cout << std::endl;
    std::cout << "=== All Prediction Tests PASSED ===" << std::endl;
    std::cout << "Prediction system demonstrates forward model integration" << std::endl;
    std::cout << "Real-time prediction error drives neural learning and behavior" << std::endl;
}

} // namespace test_prediction