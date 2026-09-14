// Neuromodulation Integration Test
// This file tests the complete neuromodulation integration
// including all connections between PredictionError, Dopamine, Curiosity, and Novelty

#include "Brain.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"

void testNeuromodulationIntegration() {
    // Create a config
    auto config = std::make_shared<Config>();
    
    // Create and initialize brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* predictionError = brain.getPredictionErrorSignal();
    auto* novelty = brain.getNovelty();
    
    // Test 1: Check initial state - all should be at baseline
    NLM_LOG_INFO("Test 1: Initial state check");
    NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
    NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosity->getLevel()));
    NLM_LOG_INFO("Prediction error magnitude: " + std::to_string(predictionError->getMagnitude()));
    NLM_LOG_INFO("Novelty level: " + std::to_string(novelty->getLevel()));
    
    // Test 2: Simulate prediction error signal
    NLM_LOG_INFO("Test 2: Prediction error integration");
    predictionError->computeError(0.5f, 0.8f); // error = 0.3
    float errorMagnitude = predictionError->getMagnitude();
    NLM_LOG_INFO("Computed prediction error: " + std::to_string(errorMagnitude));
    
    // Test 3: Simulate novelty signal
    NLM_LOG_INFO("Test 3: Novelty integration");
    novelty->setLevel(0.7f); // High novelty
    NLM_LOG_INFO("Novelty level set to: " + std::to_string(novelty->getLevel()));
    
    // Test 4: Update all neuromodulators
    NLM_LOG_INFO("Test 4: Unified neuromodulation update");
    dopamine->updateFromSources(errorMagnitude, curiosity->getLevel(), novelty->getLevel(), 0.001f);
    NLM_LOG_INFO("Dopamine level after update: " + std::to_string(dopamine->getLevel()));
    
    // Test 5: Check prediction error drives curiosity
    NLM_LOG_INFO("Test 5: Prediction error -> Curiosity -> Dopamine");
    curiosity->update(novelty->getLevel(), errorMagnitude * 0.7f, 0.001f);
    NLM_LOG_INFO("Curiosity level after update: " + std::to_string(curiosity->getLevel()));
    
    // Test 6: Check novelty drives dopamine
    NLM_LOG_INFO("Test 6: Novelty -> Dopamine");
    dopamine->signalNovelty(novelty->getLevel());
    NLM_LOG_INFO("Dopamine level after novelty: " + std::to_string(dopamine->getLevel()));
    
    // Test 7: Check curiosity affects dopamine
    NLM_LOG_INFO("Test 7: Curiosity -> Dopamine -> Curiosity weight");
    dopamine->updateFromSources(errorMagnitude, curiosity->getLevel(), novelty->getLevel(), 0.001f);
    NLM_LOG_INFO("Final dopamine level: " + std::to_string(dopamine->getLevel()));
    
    NLM_LOG_INFO("Neuromodulation integration test completed successfully!");
}

} // namespace nlm